/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGL2PSExporter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGL2PSExporter
 * @brief   export a scene as a PostScript file using GL2PS.
 *
 * vtkGL2PSExporter is a concrete subclass of vtkExporter that writes
 * high quality vector PostScript (PS/EPS), PDF or SVG files by using
 * GL2PS.  GL2PS can be obtained at: http://www.geuz.org/gl2ps/.  This
 * can be very useful when one requires publication quality pictures.
 * This class works best with simple 3D scenes and most 2D plots.
 * Please note that GL2PS has its limitations since PostScript is not
 * an ideal language to represent complex 3D scenes.  However, this
 * class does allow one to write mixed vector/raster files by using
 * the Write3DPropsAsRasterImage ivar.  Please do read the caveats
 * section of this documentation.
 *
 * By default vtkGL2PSExporter generates Encapsulated PostScript (EPS)
 * output along with the text in portrait orientation with the
 * background color of the window being drawn.  The generated output
 * is also compressed using zlib. The various other options are set to
 * sensible defaults.
 *
 * The output file format (FileFormat) can be either PostScript (PS),
 * Encapsulated PostScript (EPS), PDF, SVG or TeX.  The file extension
 * is generated automatically depending on the FileFormat.  The
 * default is EPS.  When TeX output is chosen, only the text strings
 * in the plot are generated and put into a picture environment.  One
 * can turn on and off the text when generating PS/EPS/PDF/SVG files
 * by using the Text boolean variable.  By default the text is drawn.
 * The background color of the renderwindow is drawn by default.  To
 * make the background white instead use the DrawBackgroundOff
 * function.  Landscape figures can be generated by using the
 * LandscapeOn function.  Portrait orientation is used by default.
 * Several of the GL2PS options can be set.  The names of the ivars
 * for these options are similar to the ones that GL2PS provides.
 * Compress, SimpleLineOffset, Silent, BestRoot, PS3Shading and
 * OcclusionCull are similar to the options provided by GL2PS.  Please
 * read the function documentation or the GL2PS documentation for more
 * details.  The ivar Write3DPropsAsRasterImage allows one to generate
 * mixed vector/raster images.  All the 3D props in the scene will be
 * written as a raster image and all 2D actors will be written as
 * vector graphic primitives.  This makes it possible to handle
 * transparency and complex 3D scenes.  This ivar is set to Off by
 * default.  Specific 3D props can be excluded from the rasterization
 * process by adding them to the RasterExclusions ivar.  Props in this
 * collection will be rendered as 2D vector primitives instead.
 *
 * @warning
 * By default (with Write3DPropsAsRasterImage set to Off) exporting
 * complex 3D scenes can take a long while and result in huge output
 * files.  Generating correct vector graphics output for scenes with
 * transparency is almost impossible.  However, one can set
 * Write3DPropsAsRasterImageOn and generate mixed vector/raster files.
 * This should work fine with complex scenes along with transparent
 * actors.
 *
 * @sa
 * vtkExporter
 *
 * @par Thanks:
 * Thanks to Goodwin Lawlor and Prabhu Ramachandran for this class.
*/

#ifndef vtkGL2PSExporter_h
#define vtkGL2PSExporter_h

#include "vtkIOExportGL2PSModule.h" // For export macro
#include "vtkExporter.h"

#include "vtkNew.h" // For vtkNew

class vtkPropCollection;

class VTKIOEXPORTGL2PS_EXPORT vtkGL2PSExporter : public vtkExporter
{
public:
  static vtkGL2PSExporter *New();
  vtkTypeMacro(vtkGL2PSExporter, vtkExporter)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify the prefix of the files to write out. The resulting filenames
   * will have .ps or .eps or .tex appended to them depending on the
   * other options chosen.
   */
  vtkSetStringMacro(FilePrefix);
  vtkGetStringMacro(FilePrefix);
  //@}

  //@{
  /**
   * The initial size of the GL2PS export buffer in bytes. The buffer is used to
   * store the exported image prior to writing to file. If the buffer is too
   * small, the exporter will enlarge the buffer and rerender until the export
   * is successful. Setting a larger value here can reduce the time needed to
   * export a complex scene by reducing the number of iterations required.
   * The default initial size is 4 MB.
   */
  vtkSetMacro(BufferSize, int);
  vtkGetMacro(BufferSize, int);
  //@}

  //@{
  /**
   * Set the title for the output, if supported. If nullptr, "VTK GL2PS Export" is
   * used.
   */
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);
  //@}

  enum OutputFormat
  {
      PS_FILE,
      EPS_FILE,
      PDF_FILE,
      TEX_FILE,
      SVG_FILE
  };

  //@{
  /**
   * Configure the exporter to expect a painter-ordered 2D rendering, that is,
   * a rendering at a fixed depth where primitives are drawn from the bottom up.
   * This disables sorting, which will break the painter ordering, and turns off
   * the simple line offset, which can cause line primitives to be drawn on top
   * of all other geometry.
   */
  void UsePainterSettings()
  {
    this->SetSortToOff();
    this->SetSimpleLineOffset(0);
  }
  //@}

  //@{
  /**
   * Specify the format of file to write out.  This can be one of:
   * PS_FILE, EPS_FILE, PDF_FILE, TEX_FILE.  Defaults to EPS_FILE.
   * Depending on the option chosen it generates the appropriate file
   * (with correct extension) when the Write function is called.
   */
  vtkSetClampMacro(FileFormat, int, PS_FILE, SVG_FILE);
  vtkGetMacro(FileFormat, int);
  void SetFileFormatToPS()
    {this->SetFileFormat(PS_FILE);};
  void SetFileFormatToEPS()
    {this->SetFileFormat(EPS_FILE);};
  void SetFileFormatToPDF()
    {this->SetFileFormat(PDF_FILE);};
  void SetFileFormatToTeX()
    {this->SetFileFormat(TEX_FILE);};
  void SetFileFormatToSVG()
    {this->SetFileFormat(SVG_FILE);};
  const char *GetFileFormatAsString();
  //@}

  enum SortScheme
  {
      NO_SORT=0,
      SIMPLE_SORT=1,
      BSP_SORT=2
  };

  //@{
  /**
   * Set the type of sorting algorithm to order primitives from
   * back to front.  Successive algorithms are more memory
   * intensive.  Simple is the default but BSP is perhaps the best.
   */
  vtkSetClampMacro(Sort, int, NO_SORT, BSP_SORT);
  vtkGetMacro(Sort,int);
  void SetSortToOff()
    {this->SetSort(NO_SORT);};
  void SetSortToSimple()
    {this->SetSort(SIMPLE_SORT);};
  void SetSortToBSP()
    {this->SetSort(BSP_SORT);};
  const char *GetSortAsString();
  //@}

  //@{
  /**
   * Turn on/off compression when generating PostScript or PDF
   * output. By default compression is on.
   */
  vtkSetMacro(Compress, vtkTypeBool);
  vtkGetMacro(Compress, vtkTypeBool);
  vtkBooleanMacro(Compress, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off drawing the background frame.  If off the background
   * is treated as white.  By default the background is drawn.
   * On the OpenGL2 backend, the background is always drawn.
   */
  vtkSetMacro(DrawBackground, vtkTypeBool);
  vtkGetMacro(DrawBackground, vtkTypeBool);
  vtkBooleanMacro(DrawBackground, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off the GL2PS_SIMPLE_LINE_OFFSET option.  When enabled a
   * small offset is added in the z-buffer to all the lines in the
   * plot.  This results in an anti-aliasing like solution.  Defaults to
   * on.
   */
  vtkSetMacro(SimpleLineOffset, vtkTypeBool);
  vtkGetMacro(SimpleLineOffset, vtkTypeBool);
  vtkBooleanMacro(SimpleLineOffset, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off GL2PS messages sent to stderr (GL2PS_SILENT).  When
   * enabled GL2PS messages are suppressed.  Defaults to off.
   */
  vtkSetMacro(Silent, vtkTypeBool);
  vtkGetMacro(Silent, vtkTypeBool);
  vtkBooleanMacro(Silent, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off the GL2PS_BEST_ROOT option.  When enabled the
   * construction of the BSP tree is optimized by choosing the root
   * primitives leading to the minimum number of splits.  Defaults to
   * on.
   */
  vtkSetMacro(BestRoot, vtkTypeBool);
  vtkGetMacro(BestRoot, vtkTypeBool);
  vtkBooleanMacro(BestRoot, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off drawing the text.  If on (default) the text is drawn.
   * If the FileFormat is set to TeX output then a LaTeX picture is
   * generated with the text strings.  If off text output is
   * suppressed.
   */
  vtkSetMacro(Text, vtkTypeBool);
  vtkGetMacro(Text, vtkTypeBool);
  vtkBooleanMacro(Text, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off landscape orientation.  If off (default) the
   * orientation is set to portrait.
   */
  vtkSetMacro(Landscape, vtkTypeBool);
  vtkGetMacro(Landscape, vtkTypeBool);
  vtkBooleanMacro(Landscape, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off the GL2PS_PS3_SHADING option.  When enabled the
   * shfill PostScript level 3 operator is used.  Read the GL2PS
   * documentation for more details.  Defaults to on.
   */
  vtkSetMacro(PS3Shading, vtkTypeBool);
  vtkGetMacro(PS3Shading, vtkTypeBool);
  vtkBooleanMacro(PS3Shading, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off culling of occluded polygons (GL2PS_OCCLUSION_CULL).
   * When enabled hidden polygons are removed.  This reduces file size
   * considerably.  Defaults to on.
   */
  vtkSetMacro(OcclusionCull, vtkTypeBool);
  vtkGetMacro(OcclusionCull, vtkTypeBool);
  vtkBooleanMacro(OcclusionCull, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off writing 3D props as raster images.  2D props are
   * rendered using vector graphics primitives.  If you have hi-res
   * actors and are using transparency you probably need to turn this
   * on.  Defaults to Off.
   * This option has no effect when the OpenGL2 backend is in use, as all
   * 3D props (excluding vtkTextActor3D) are rasterized into the background.
   */
  vtkSetMacro(Write3DPropsAsRasterImage, vtkTypeBool);
  vtkGetMacro(Write3DPropsAsRasterImage, vtkTypeBool);
  vtkBooleanMacro(Write3DPropsAsRasterImage, vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off exporting text as path information, rather than character data.
   * This is useful for the PDF backend, which does not properly
   * support aligned text otherwise. Defaults to Off.
   */
  vtkSetMacro(TextAsPath, bool);
  vtkGetMacro(TextAsPath, bool);
  vtkBooleanMacro(TextAsPath, bool);
  //@}

  //@{
  /**
   * Collection of props to exclude from rasterization. These will be
   * rendered as 2D vector primitives in the output. This setting is ignored if
   * Write3DPropsAsRasterImage is false. Behind the scenes, these props are
   * treated as 2D props during the vector output generation.
   */
  void SetRasterExclusions(vtkPropCollection*);
  vtkGetObjectMacro(RasterExclusions, vtkPropCollection);
  //@}

  //@{
  /**
   * Set the ratio between the OpenGL PointSize and that used by GL2PS
   * to generate PostScript.  Defaults to a ratio of 5/7.
   */
  vtkSetMacro(PointSizeFactor, float);
  vtkGetMacro(PointSizeFactor, float);
  //@}

  //@{
  /**
   * Set the ratio between the OpenGL LineWidth and that used by GL2PS
   * to generate PostScript.  Defaults to a ratio of 5/7.
   */
  vtkSetMacro(LineWidthFactor, float);
  vtkGetMacro(LineWidthFactor, float);
  //@}

protected:
  vtkGL2PSExporter();
  ~vtkGL2PSExporter() override;

  int GetGL2PSOptions();
  int GetGL2PSSort();
  int GetGL2PSFormat();
  const char *GetFileExtension();

  vtkPropCollection *RasterExclusions;

  char *FilePrefix;
  char *Title;
  int FileFormat;
  int BufferSize;
  int Sort;
  vtkTypeBool Compress;
  vtkTypeBool DrawBackground;
  vtkTypeBool SimpleLineOffset;
  vtkTypeBool Silent;
  vtkTypeBool BestRoot;
  vtkTypeBool Text;
  vtkTypeBool Landscape;
  vtkTypeBool PS3Shading;
  vtkTypeBool OcclusionCull;
  vtkTypeBool Write3DPropsAsRasterImage;
  bool TextAsPath;
  float PointSizeFactor;
  float LineWidthFactor;

private:
  vtkGL2PSExporter(const vtkGL2PSExporter&) = delete;
  void operator=(const vtkGL2PSExporter&) = delete;
};

inline const char *vtkGL2PSExporter::GetSortAsString(void)
{
  if ( this->Sort == NO_SORT )
  {
    return "Off";
  }
  else if ( this->Sort == SIMPLE_SORT )
  {
    return "Simple";
  }
  else
  {
    return "BSP";
  }
}

inline const char *vtkGL2PSExporter::GetFileFormatAsString(void)
{
  if ( this->FileFormat == PS_FILE )
  {
    return "PS";
  }
  else if ( this->FileFormat == EPS_FILE )
  {
    return "EPS";
  }
  else if ( this->FileFormat == PDF_FILE )
  {
    return "PDF";
  }
  else if ( this->FileFormat == TEX_FILE )
  {
    return "TeX";
  }
  else
  {
    return "SVG";
  }
}

#endif
