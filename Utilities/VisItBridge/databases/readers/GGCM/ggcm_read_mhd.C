/** Module to read run length encoded ASCII MHD files generated by
 * the GGCM code.  These subroutines were taken from Bill Sherman's
 * "convertmhd.c" code, which was designed to create VTK formatted
 * output files.
 *
 * NOTE:
 *   Compiling works under both Linux and IRIX.  However, the Linux output
 *   seems to round up a bit higher than the IRIX.  Ie. the last significant
 *   digit in the Linux output may be one numeral higher than IRIX.  Since
 *   we're dealing with values that have some loss from the MHD compression
 *   it probably doesn't matter much -- but it makes it difficult to compare
 *   outputs with "diff".
 *
 * HISTORY:
 *   This code was initially created by Bill Sherman in late 2003.  John C.
 *   Dorelli cleaned things up a bit and made it easier to use for us GGCM
 *   people.  Maintainence has since been handed to Tom Fogal, who has turned
 *   the program into a library and done other various fixes.
 *   The old history that was here has been removed; most of it didn't make
 *   sense anymore anyway.  You can grep it out of the subversion logs / old
 *   versions if you really want it.
 ****/

//   Jeremy Meredith, Wed Jan  6 17:46:31 EST 2010
//   Added VisIt exceptions in place of aborts.
//

#include "libggcm.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <InvalidFilesException.h>

/** fscanf that is expected to work; aborts on error */
#define SCAN(fp, match, v1)                   \
    {                                         \
        int err;                              \
        err = fscanf(fp, match, v1);          \
        if(err == EOF) {                      \
            perror("scanning in libggcm");    \
            EXCEPTION1(InvalidFilesException, mhd->filename); \
        }                                     \
    }
/** more stringent; give the number of arguments which must match. */
#define NSCAN(fp, num, match, v1, v2, v3, v4, v5, v6)     \
    {                                                     \
        int err;                                          \
        err = fscanf(fp, match, v1, v2, v3, v4, v5, v6);  \
        if(err == EOF || err != num) {                    \
            perror("scanning in libggcm");                \
            fprintf(stderr, "%d matches\n", err);         \
            EXCEPTION1(InvalidFilesException, mhd->filename); \
        }                                                 \
    }

static int wrndec(unsigned char *in_string, int *out_array);
static int readRLEData(MHDdata *data);
static int field_match(const char * const match_list[],
                       const char * const field_name);
static int readMHDField(MHDdata *data, char *field_info,
                        const char * const match_list[]);
static int read_time_line(MHDdata *);
static int ggcm_read_field_metadata(MHDdata *);

/** alias for ggcm_init_mhd */
void initMHD(MHDdata *data) { ggcm_init_mhd(data); }

/** alias for ggcm_read_mhd */
MHDdata *readMHDFile(const char *file_name, const char *field_list[], long file_start)
{
    return ggcm_read_mhd(file_name, field_list);
}

/** alias for ggcm_free_data */
void freeMHDData(MHDdata *md) { ggcm_free_data(md); }

/** initialize an MHDdata record */
void ggcm_init_mhd(MHDdata *data)
{
    data->next = NULL;
    data->field_name[0] = '\0';

    data->regular = 1;
    data->subsample[0] = 1;
    data->subsample[1] = 1;
    data->subsample[2] = 1;
    data->subsamp_dim[0] = 1;
    data->subsamp_dim[1] = 1;
    data->subsamp_dim[2] = 1;

    data->line3_info[0] = '\0';
    data->line4_value = 0;

    data->filename[0] = '\0';
    data->mhd_fp = NULL;
    data->file_position = -1;

    data->dim_rank = 0;
    data->dim_size[0] = 1;
    data->dim_size[1] = 1;
    data->dim_size[2] = 1;
    data->value_rank = 1;
    data->number = 0;
    data->write_data = 1;
    data->num_covecs = 0;
    data->data = NULL;
    data->grid[0] = NULL;
    data->grid[1] = NULL;
    data->grid[2] = NULL;
    data->co_vector[0] = NULL;
    data->co_vector[1] = NULL;
    data->co_vector[2] = NULL;
}

/** creates a list which contains all of the metadata for a given GGCM file,
 * such as the fields contained within it, data points / field, time code
 * information, etc. */
MHDdata *ggcm_read_metadata(const char *filename)
{
    MHDdata *head, *cur, *prev;   /* linked list of field information */
    unsigned int lines, i;        /* how many lines to skip, current line */
    size_t len;                   /* length of temp buffer */
    char *line;                   /* temp buffer for junking lines */
    int fields = 0;               /* more fields to read? */

    head = prev = cur = (MHDdata *)malloc(sizeof(MHDdata));
    head->mhd_fp = fopen(filename, "r");
    strcpy(head->filename,filename); // for error reporting

    len = 1024;
    line = (char *)malloc(len);
    while((fields = ggcm_read_field_metadata(cur)) != 0) {
        prev = cur;
        /* how many lines should we skip? */
        lines = ((cur->number % 64) != 0) ?
                ((cur->number/64)+1) : (cur->number/64);
        lines *= 2;

        /* 'seek' to the next field in the file.  We'd like to be able to use
         * fseek here, but we only know how many *lines* the data are, not how
         * many *bytes* they are.  So we have to scan every line... */
        for(i=0; i < lines; ++i) {
            /* fgets works pretty well too; getline is slightly faster.
               fscanf is extremely slow.  Using fgets since it is portable,
               whereas getline is only on linux. */
            char* res = fgets(line, len, cur->mhd_fp); (void) res;
        }

        cur->next = (MHDdata *)malloc(sizeof(MHDdata)); /* need another elem in the list */
        cur->next->mhd_fp = cur->mhd_fp;     /* field_metadata needs a good FP */

        cur = cur->next;
        cur->next = NULL; /* NULL for the sentinel */
    }

    /* bogus extra element */
    free(cur);
    prev->next = NULL;

    free(line); /* buffer for getline */
    fclose(head->mhd_fp);
    return head;
}

/** reads the metadata for a given field.
 * @todo add a field to MHDdata records which includes the file offset (via
 *       ftell(3)) of the field in question, and set that here right after
 *       reading the last bit of information ("line 5").  This will allow a
 *       more efficient reader when I rewrite that in the future, since it can
 *       know exactly where to seek to. */
static int ggcm_read_field_metadata(MHDdata *mhd)
{
    char junk[512];

    /* read the first field header, describing the field type (2D? 3D? etc.).
     * If this read fails, we're probably at EOF, indicating there are no more
     * fields. */
    int res = fscanf(mhd->mhd_fp, "FIELD-%[^\n ]\n", junk); (void) res;
    if(ferror(mhd->mhd_fp)) {
        perror("Unrecoverable reading field metadata");
        EXCEPTION1(InvalidFilesException, mhd->filename);
    }
    if(feof(mhd->mhd_fp)) {
        return 0;
    }

    SCAN(mhd->mhd_fp, "%s\n", mhd->field_name);
    read_time_line(mhd);

    /* line 4 has info on dimensionality, etc; ignored for now. */
    SCAN(mhd->mhd_fp, "%[^\n]\n", junk);

    /* the important thing on line 5 is the 'number' -- how many data points are
     * in this field. */
    NSCAN(mhd->mhd_fp, 6, "WRN2 %d%f %f %f %d%[^\n]\n", &mhd->number,
                          &mhd->wrn2_z1, &mhd->wrn2_z2, &mhd->wrn2_rid,
                          &mhd->wrn2_it, mhd->wrn2_cid);
    return 1;
}

/** wrndec(): decodes a line of run-length encoded ascii data into an
 *   integer array ("out_array") which is returned via call-by-reference.
 *   The length of the output array is returned normally.  The input to
 *   this function is a character array of the string to decode.
 * @param in_string characters to decode (nee "c" & replacement for "iu")
 * @param out_array integer array number 1 -- return values (nee "i1")
 * @return the number of values (nee "n+1") */
int wrndec(unsigned char *in_string, int *out_array)
{
    int    index_out = 0;    /* index into the outgoing "out_array" array (nee "i")  */
    int    index_in = 0;    /* index into the "in_string" character array (nee "j") */
    int    input_checksum;    /* checksum of the incoming character string (nee i2[0])*/
    int    checksum;    /* checksum value (nee "ick") */
    int    run_value;    /* running value (nee "ir") */
    int    run_count;    /* ren-length count (nee "ic") */
    int    count;

    /* set some initial values in case of error */
    for (count = 0; count < 64; count++) {
        out_array[count] = 34;        /* NOTE: why 34?  seems like "33" would be better */
    }

    input_checksum = in_string[0];
    checksum = 0;

    index_in = 1; /* because C is 0 based, and perhaps the original code is 1-based */
    while ((run_value = (int)(in_string[index_in])) != 0) {
        /* NOTE: in Fortran code the value "32" is the terminal value.  Need  */
        /*   to find out what significance that value has related to strings. */

        if (run_value < 0) {
            printf("adjusting sign -- preferable to deal with unsigned ints: run_value = %d\n", run_value);
            run_value += 256; /* shouldn't this be an abs() call? */
        }

        if (run_value > 127) {
            /* this means the value is actually a run-count, so get the   */
            /*   count, and the next number is the actual repeating value */
            run_count = run_value-170;
            run_value = (int)(in_string[++index_in]);
            if (run_value < 0) {
                printf("adjusting sign (2) -- preferable to deal with unsigned ints: run_value = %d\n", run_value);
                run_value += 256;
            }

            for (count = 0; count < run_count; count++) {
                if (index_out < 67)
                    out_array[index_out++] = run_value;
                else    printf("wrndec(): catastrophic error -- trying to write outside the output array\n");
                checksum += run_value;
            }
        } else {
            if (index_out < 67)
                out_array[index_out++] = run_value;
            else    printf("wrndec(): catastrophic error -- trying to write outside the output array\n");
            checksum += run_value;
        }

        index_in++;
        if (index_in > 67) {
            printf("error:wrndec: can't find end of encoded record -- index_in = %d\n", index_in);
            return index_out;
        }
    }

    if (index_out > 64) {
        printf("error: wrndec: range error -- index_out(%d) > 64\n", index_out);
        return 64;
    }
    if (input_checksum != 33 + (checksum % 92)) {
        printf("error: wrndec: checksum error, mismatch: %d vs %d -- index_out = %d\n", input_checksum, 33 + (checksum % 92), index_out);
        return 0;
    } else {
#if 0
        printf("checksum match: %d\n", input_checksum);
#endif
    }

    return index_out;
}

/** readRLEData(): Read a run-length ascii encoding with N=2 bytes
 * Reads ascii-encoded run-length data from the file pointed to by
 * data->mhd_fp.  The "data" structure is also used to pass in values
 * of the "wrn" encoding, such as the the min/max range of the absolute
 * values, and the number and organization of the data (though this
 * routine could really care less about the organization).
 *
 * In the original (util1.for) version, this routine is responsible for
 *   reading "line 5", however in my version, I've already read and
 *   parsed that info.
 * returns number of elements (aka "n" in the fortran code)
 *
 * NOTE: in rmhd06/util.for, the rdn2() procedure takes six arguments:
 *   iu  -- file id (called "data->mhd_fp" here)
 *   a   -- data array (floats) (called "data->data" here)
 *   n   -- number of elements [returned value?] (called "data->number")
 *   cid -- type of data (called "data->wrn2_cid" here)
 *   it  -- [no longer used] (was timecode)
 *   rid -- [not used]
 *
 * Modifications:
 *   Mark C. Miller, Thu Nov  6 11:12:56 PST 2008
 *   Fixed compilation warning on line ~389 converting double to int
 **/
int readRLEData(MHDdata *data)
{
    unsigned char buffer[128];    /* buffer for input string -- from wrndec()         */
    int    count;
    int    block64;    /* for counting through the elements 64 numbers at a time (nee "k") */
    int    block_size;    /* number of expected values to be returned by wrndec() -- (nee "nk+1") */
    int    msbits[72];    /* integer array of most significant bits decoded by wrndec() (nee "i1") */
    int    lsbits[72];    /* integer array of least significant bits decoded by wrndec() (nee "i2") */
    int    int_value;    /* reconstructed integer value with all the bits (nee "i3" array) */
                /*   NOTE: this doesn't need to be an array.                */
#if 0 /* fltarray_1 isn't necessary because it gets immediately stored in the actual data array */
    float    fltarray_1[64];    /* float array that decodes one block of data (nee "a1")*/
#endif
    int    num_decoded;    /* value of "n" as returned by wrndec() (nee "nn") */
    float    dzi;        /* some sort of dynamic range value */
    float    sign;        /* the sign of the result (nee "sig") */
#if 0 /* data_index is no longer necessary, because it is equivalent to "count+block64" */
    int    data_index = 0;    /* index into the full data array of where to write next (nee "l") */
#endif
    int    scan;        /* return value from call to fscanf */
    int    bytes_read;    /* number of bytes read by call to fscanf */

    /* Free the data array memory if it exists */
    if (data->data != NULL)
        free(data->data);

    /* Need to allocate the memory for the data array */
    data->data = (float *)calloc(data->number, sizeof(float));

    /* If the min and the max are the same, then all elements are that value. */
    if(data->wrn2_z1 == data->wrn2_z2) {
        for (count = 0; count < data->number; count++)
            data->data[count] = data->wrn2_z1;
        return data->number;
    }

    /* The decoding is done in 64 element chunks (each stored in two separate byte arrays) */
    for(block64 = 0; block64 < data->number; block64 += 64) {
        /* set block_size to the number of elements expected from the decoder */
        /*   (either the number of remaining elements, or a cap of 64).       */
        block_size = data->number - block64;
        if (block_size > 64)
            block_size = 64;

        /* decode one block of input (if all is well, this will match the expected block_size) */

        /* read one line from input file and then decode it */
        scan = fscanf(data->mhd_fp, "%[^\n]%n\n", buffer, &bytes_read);
        if(scan < 0) {
            fprintf(stderr, "Difficulting reading from MHD file.\n");
        }
        num_decoded = wrndec(buffer, msbits);
        if(num_decoded != block_size) {
#ifdef DEBUG_READ_VERBOSE
            printf("readRLEData[1]: num_decoded (%d) != block_size (%d) [number = %d, block64 = %d]\n",
                   num_decoded, block_size, data->number, block64);
#endif
            data->number = -2;    /* return an error code */
            return data->number;
        }

        /* read one line from input file and then decode it */
        scan = fscanf(data->mhd_fp, "%[^\n]%n\n", buffer, &bytes_read);
        if(scan < 0) {
            fprintf(stderr, "Difficulting reading from MHD file.\n");
        }
        num_decoded = wrndec(buffer, lsbits);
        if(num_decoded != block_size) {
#ifdef DEBUG_READ_VERBOSE
            printf("readRLEData[2]: num_decoded (%d) != block_size (%d) [number = %d, block64 = %d]\n",
                   num_decoded, block_size, data->number, block64);
#endif
            data->number = -2;    /* return an error code */
            return data->number;
        }

        /* Okay, we've decoded the MSbits and the LSbits of */
        /* data now convert them into floating point values */
        dzi = (data->wrn2_z2 - data->wrn2_z1) / 4410.0;
        for (count = 0; count < block_size; count++) {
            /* subtract the 33 that was added to put it above the ascii control-characters */
            msbits[count] -= 33;
            lsbits[count] -= 33;

            /* the sign is stored in msbits[] as the 47-or-greater-bit */
            sign = 1.0;
            if (msbits[count] >= 47) {
                sign = -1.0;
                msbits[count] -= 47;
            }

            /* reconstruct the unsigned integer value */
            int_value = (int) (lsbits[count] + 94.0 * msbits[count]);

            /* reconstruct the original floating point value from the range information & sign */
            /* Note that the following expression uses float(exp()) instead */
            /* of expf(). This is because we are using gcc 3.2 on a solaris */
            /* machine, which doesn't support expf. We should change this */
            /* when we no longer support gcc 3.2. */
           
            data->data[block64+count] = sign * float(exp(dzi * int_value + data->wrn2_z1));
        }
    }

    return data->number;
}

/** field_match(): returns a boolean value of whether the given field
 * name is in the list of requested fields.  If found, the name is
 * removed from the list, and a 1 is returned.  Otherwise 0 is
 * returned.  If the value of the match_list is NULL, then it is
 * assumed that all fields should be read, and therefore 1 is
 * returned */
int field_match(const char * const match_list[], const char * const field_name)
{
    int    count;
    int    match = 0;    /* no match found so far */
    if (match_list == NULL)
        return 1;

#if 0
    char *check;
    for(check = match_list[count = 0]; check != NULL; check = match_list[++count]) {
        if(strcmp(check, field_name) == 0) {
            match = 1;
        }
    }
#else
    for(count = 0; match_list[count] != NULL; ++count) {
        if(strcmp(match_list[count], field_name) == 0) {
            match = 1;
            break;
        }
    }
#endif
    return match;
}

/** readMHDField() reads the next wrn ASCII-encoded MHD data field into
 * the MHDdata structure
 *
 * NOTE:  the first line of the field is given by the "field_info"
 *  argument.  The rest of the data is from the "mhd_fp" field of
 *  MHDField -- though that may change in the (near) future.
 *  The header information and a pointer to the data array are
 *  returned via the MHDdata structure
 *
 * The format of the ASCII field representation is 5 lines of header
 *  info, followed by approximately (number-of-elements/32) lines
 *  of WRN ASCII encoded data.
 *    Line 1: ...
 *    Line 2: ...
 *    Line 3: ...
 *    Line 4: ...
 *    Line 5: ... */
int readMHDField(MHDdata *data, char *field_info,
                 const char * const match_list[])
{
    char    junk[256];
    int    scan;          /* return value from a scanf() call */
    int    rank;
    int    num_elements;  /* return value of this function */

    /* determine the beginning position of the file */
    data->file_position = ftell(data->mhd_fp);
    data->file_position -= (7 + strlen(field_info));

    /* Line 1: the number of dimensions, and rank of each element */
    scan = sscanf(field_info, "%dD-%d%[^\n]\n", &data->dim_rank, &data->value_rank, junk);
    if (scan != 2) {
        printf("readMHDField() trouble scanning line 1, got %d values instead of 2\n", scan);
    }

    /* Line 2: the name of this field */
    scan = fscanf(data->mhd_fp, "%[^\n ]\n", &data->field_name[0]);
    /* scan = fscanf(data->mhd_fp, "%[a-z]\n", &data->field_name[0]) */;
    if (scan != 1) {
        printf("readMHDField() trouble scanning line 2, got %d values instead of 1\n", scan);
    }
#ifdef DEBUG_READ_VERBOSE
    printf("reading Field '%s' of type '%s'\n", data->field_name, field_info);
#endif

    if(strcmp(data->field_name,"bx") == 0 ||
       strcmp(data->field_name,"by") == 0 ||
       strcmp(data->field_name,"bz") == 0 ||
       strcmp(data->field_name,"vx") == 0 ||
       strcmp(data->field_name,"vy") == 0 ||
       strcmp(data->field_name,"vz") == 0 ||
       strcmp(data->field_name,"xjx") == 0 ||
       strcmp(data->field_name,"xjy") == 0 ||
       strcmp(data->field_name,"xjz") == 0 ||
       strcmp(data->field_name,"rr") == 0 ||
       strcmp(data->field_name,"pp") == 0 ||
       strcmp(data->field_name,"resis") == 0) {
        /* then Line 3 is time data. (TJF) */
        read_time_line(data);
#if DEBUG
        fprintf(stderr, "read time data: %lf, %lf\n\t %u:%u:%u:%u:%u:%lf\n",
                        data->time_since_model_start,
                        data->some_time,
                        data->year, data->month, data->day,
                        data->hour, data->minute, data->second);
#endif
    } else {
        /* then Line 3 is the name of the grid file, or something else we don't
         * care about. */
        scan = fscanf(data->mhd_fp, "%[^\n]\n", &data->line3_info[0]);
        if(scan != 1) {
            printf("readMHDField() trouble scanning line 3, got ");
            printf("%d values instead of 1\n", scan);
        }
    }

    /* Line 4: info about the dimensionality of the data */
    scan = fscanf(data->mhd_fp, " %d", &data->line4_value);
    if (scan != 1) {
        printf("readMHDField() trouble scanning line 4a, got %d values instead of 1\n", scan);
    }
    for (rank = 0; rank < data->dim_rank; rank++) {
      scan = fscanf(data->mhd_fp, " %d", &data->dim_size[rank]);
        if (scan != 1) {
            printf("readMHDField() trouble scanning line 4b, got %d values instead of 1\n", scan);
        }
    }
    scan = fscanf(data->mhd_fp, "%[^\n]\n", junk);  /* go to beginning of next line */
    /* Line 5: The WRN2 line */
    /* NOTE: perhaps the "2" in WRN2 is an indicator of the number of bytes */
    /*   into which the values are stored.                                  */
    /* Sometimes "WRN2" (data->encoding) is not separated from the # of values
     *  (data->number) via a space. not sure why, but the original code gets
     *  screwed up because it tries to read everything in one go. This is a
     * slightly modified fscanf that limits the first field to 4 chars */
    scan = fscanf(data->mhd_fp, "%4s %d%f %f %f %d%[^\n]\n", data->encoding,
                  &data->number, &data->wrn2_z1, &data->wrn2_z2,
                  &data->wrn2_rid, &data->wrn2_it, &data->wrn2_cid[0]);
    if(scan != 7) {
        printf("readMHDField() trouble scanning line 5, got %d values instead of 7\n", scan);
    }
    /* Lines 6 to ~(data->number / 32): the ascii encoded data */
    /*   Only read the data if this field matches a requested field, otherwise skip */
    if(!field_match(match_list, data->field_name)) {
#ifdef DEBUG_READ
      printf("readMHDField() No match.\n");
#endif
        int    count;    /* count through the lines */
        int    skip;    /* number of lines to skip */

        skip = data->number / 64;
        if(data->number % 64 != 0)
            skip++;
        skip *= 2;
        for(count = 0; count < skip; count++) {
            int res = fscanf(data->mhd_fp, "%*[^\n]\n"); (void) res; /* read one line */
        }

        num_elements = 0;
    } else {
#ifdef DEBUG_READ
      printf("readMHDField() reading data\n");
#endif
        num_elements = readRLEData(data);
    }
    return num_elements;
}

/* returns the mhd file in a linked list sort of structure called 'MHDdata'. As
 * far as I can tell, the structure itself is dynamically allocated, as well as
 * the 'data' pointer within each element. The calling program will need to
 * deallocate these.
 * 'field_list' also seems to be modified by this function or another it calls.
 * At best, this causes a memory leak. Worse, usually field_list ends up being
 * allocated on the stack... this might cause stack corruption!
 ****
 * Tom Fogal, Thu Aug 19 15:42:47 EST 2004
 *    Comments, fixes. I removed the ability to read a file from stdin. Seems
 *    an unlikely use anyway. */
MHDdata *ggcm_read_mhd(const char *file_name, const char *field_list[])
{
    FILE    *mhd_fp;   /* pointer to the MHD file to read from */
    MHDdata *data;     /* pointer to the current MHD dataset */
    MHDdata *first;    /* pointer to the first MHD dataset */
    MHDdata *last;     /* pointer to the last MHD dataset */
    char    info[256];
    int     scan;      /* return value from a scanf */

    mhd_fp = fopen(file_name, "r");
    if (mhd_fp == NULL) {
        fprintf(stderr, "Error, unable to open '%s'\n", file_name);
        EXCEPTION1(InvalidFilesException, file_name);
    }

    /* first scan the preliminary listing -- I guess that's what it is */
    while (fscanf(mhd_fp, "FIELD-%[^\n]\n", info)) {
#ifdef DEBUG_READ
        fprintf(stderr,"prelim: expect field of '%s'\n", info);
#endif
    }

    first = (MHDdata *)malloc(sizeof(MHDdata));
    last = first;
    data = first;
    ggcm_init_mhd(data);

    /* Now scan the actual data listings */
    /*   We can stop either when there are no more FIELDs to read, or */
    /*   when all the fields in the field_list have been read.        */
    while (!feof(mhd_fp) && field_list[0] != NULL) {
        scan = fscanf(mhd_fp, "FIELD-%[^\n]\n", info);
        if (scan < 0) {
            fprintf(stderr, "Difficulty reading from MHD file.\n");
        }
        data->mhd_fp = mhd_fp;
        strcpy(data->filename, (char* const)file_name);
        if(readMHDField(data, info, field_list) > 0) {
#ifdef DEBUG_READ
            printf("Just read data '%s'\n", data->field_name);
#endif
            assert(data->data); /* must have read in some data */
            /* we read some data, so there was a match with the field_list */
            data->next = (MHDdata *)malloc(sizeof(MHDdata));
            ggcm_init_mhd(data->next); /* initialize space for the next field */
            last = data;
            data = data->next;
        } else {
#ifdef DEBUG_READ
            printf("Just ignored data '%s'\n", data->field_name);
#endif
            /* if the field didn't match, then reset the values in "data" */
            ggcm_init_mhd(data);
        }
        info[0] = 'y'; info[1] = 'o'; info[2] = 0;
    }

    /* remove the link to the empty data set from the last read MHD strcuture */
    ggcm_free_data(last->next);
    last->next = NULL;

    fclose(mhd_fp);
    return first;
}

void ggcm_free_data(MHDdata *mhd)
{
    MHDdata *p, *prev;

    prev = p = mhd;
    while(p) {
        p = p->next;
        free(prev->data);
        free(prev);
        prev = p;
    };
}

static int read_time_line(MHDdata *data)
{
    char junk[256]; /* for throwing away some filename data */
    int matches;

    /* file pointer must be valid */
    assert(data != NULL);
    assert(data->mhd_fp != NULL);
    /* and have data ready for us. */
    assert(feof(data->mhd_fp) == 0 && ferror(data->mhd_fp) == 0);

    matches = fscanf(data->mhd_fp,
                     "time= %lf %lf %u:%u:%u:%u:%u:%lf %[^\n ]\n",
                     &data->time_since_model_start,  /* %lf */
                     &data->some_time,               /* %lf */
                     &data->year,                    /* %u */
                     &data->month,                   /* %u */
                     &data->day,                     /* %u */
                     &data->hour,                    /* %u */
                     &data->minute,                  /* %u */
                     &data->second,                  /* %lf */
                     junk);                          /* %[\n ] */
    if(matches != 9) {
        fputs("Error reading time code information!", stderr);
        return -1;
    }
    return 0;
}
