/*****************************************************************************
*
* Copyright (c) 2000 - 2018, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

#include <BufferConnection.h>

BufferConnection::BufferConnection() : buffer()
{
}

BufferConnection::~BufferConnection()
{
}

int
BufferConnection::Fill()
{
    return 0;
}

void
BufferConnection::Flush()
{
    // Clear the buffer
    buffer.clear();
}

long
BufferConnection::Size()
{
    return (long)(buffer.empty() ? 0 : buffer.size());
}

void
BufferConnection::Reset()
{
    buffer.clear();
}

void
BufferConnection::Write(unsigned char value)
{
    buffer.push_back(value);
} 

void
BufferConnection::Read(unsigned char *address)
{
   if(buffer.empty())
   {
       *address = 0;
   }
   else
   {
       *address = buffer.front();
       buffer.pop_front();
   }
}

void
BufferConnection::Append(const unsigned char *buf, int count)
{
    const unsigned char *temp = buf;
    for(int i = 0; i < count; ++i)
        buffer.push_back(*temp++);
}

// ****************************************************************************
// Method: BufferConnection::DirectRead
//
// Purpose: 
//   Reads the contents of the connection into the passed-in buffer.
//
// Arguments:
//    buf : The destination buffer.
//    len : The length of the destination buffer.
//
// Returns:    The number of bytes read.
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Mon Mar 25 14:21:35 PST 2002
//
// Modifications:
//
//     Burlen Loring, Mon Aug  3 13:29:43 PDT 2015
//     Fix a bug where this method did nothing.
//
// ****************************************************************************

long
BufferConnection::DirectRead(unsigned char *buf, long len)
{
    if (!buf)
        return 0;

    long n = 0;
    while (buffer.size() && (n < len))
    {
        buf[n] = buffer.front();
        buffer.pop_front();
        ++n;
    }

    return n;
}

// ****************************************************************************
// Method: BufferConnection::DirectWrite
//
// Purpose: 
//   Writes the passed-in buffer into the internal buffer.
//
// Arguments:
//   buf : The buffer to copy.
//   len : The length of the buffer to copy.
//
// Returns:    The number of bytes written.
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Mon Mar 25 14:20:11 PST 2002
//
// Modifications:
//   
// ****************************************************************************

long
BufferConnection::DirectWrite(const unsigned char *buf, long len)
{
    Append(buf, len);
    return len;
}
