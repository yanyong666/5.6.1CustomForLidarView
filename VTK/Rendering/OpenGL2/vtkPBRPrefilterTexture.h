/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPBRPrefilterTexture.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPBRPrefilterTexture
 * @brief   precompute prefilter texture used in physically based rendering
 *
 * Prefilter texture is a cubemap result of integration of the input cubemap contribution in
 * BRDF equation. The result depends on roughness coefficient so several levels of mipmap are
 * used to store results of different roughness coefficients.
 * It is used in Image Base Lighting to compute the specular part.
 */

#ifndef vtkPBRPrefilterTexture_h
#define vtkPBRPrefilterTexture_h

#include "vtkOpenGLTexture.h"
#include "vtkRenderingOpenGL2Module.h" // For export macro

class vtkOpenGLFramebufferObject;
class vtkOpenGLRenderWindow;
class vtkOpenGLTexture;
class vtkRenderWindow;

class VTKRENDERINGOPENGL2_EXPORT vtkPBRPrefilterTexture : public vtkOpenGLTexture
{
public:
  static vtkPBRPrefilterTexture* New();
  vtkTypeMacro(vtkPBRPrefilterTexture, vtkOpenGLTexture);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the input cubemap.
   */
  void SetInputCubeMap(vtkOpenGLTexture*);
  vtkGetObjectMacro(InputCubeMap, vtkOpenGLTexture);
  //@}

  /**
   * Implement base class method.
   */
  void Load(vtkRenderer*) override;

  /**
   * Implement base class method.
   */
  void Render(vtkRenderer* ren) override { this->Load(ren); }

  //@{
  /**
   * Set/Get size of texture.
   * Default is 128.
   * This value should be increased if glossy materials are present
   * in order to have better reflections.
   */
  vtkGetMacro(PrefilterSize, unsigned int);
  vtkSetMacro(PrefilterSize, unsigned int);
  //@}

  //@{
  /**
   * Set/Get the number of samples used during Monte-Carlo integration.
   * Default is 1024.
   * In some OpenGL drivers (OSMesa, old OSX), the default value might be too high leading to
   * artifacts.
   */
  vtkGetMacro(PrefilterSamples, unsigned int);
  vtkSetMacro(PrefilterSamples, unsigned int);
  //@}

  //@{
  /**
   * Set/Get the number of mip-map levels.
   * Default is 5.
   */
  vtkGetMacro(PrefilterLevels, unsigned int);
  vtkSetMacro(PrefilterLevels, unsigned int);
  //@}

protected:
  vtkPBRPrefilterTexture() = default;
  ~vtkPBRPrefilterTexture() override;

  unsigned int PrefilterSize = 128;
  unsigned int PrefilterLevels = 5;
  unsigned int PrefilterSamples = 1024;
  vtkOpenGLTexture* InputCubeMap = nullptr;

private:
  vtkPBRPrefilterTexture(const vtkPBRPrefilterTexture&) = delete;
  void operator=(const vtkPBRPrefilterTexture&) = delete;
};

#endif
