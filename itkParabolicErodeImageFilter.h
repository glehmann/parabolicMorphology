#ifndef __itkParabolicErodeImageFilter_h
#define __itkParabolicErodeImageFilter_h

#include "itkParabolicErodeDilateImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{
template <typename TInputImage,
          typename TOutputImage= TInputImage >
class ITK_EXPORT ParabolicErodeImageFilter:
    public ParabolicErodeDilateImageFilter<TInputImage, false,
					   TOutputImage>
{

public:
  /** Standard class typedefs. */
  typedef ParabolicErodeImageFilter  Self;
  typedef ParabolicErodeDilateImageFilter<TInputImage, false, TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);


  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef TOutputImage                                   OutputImageType;
  typedef typename TInputImage::PixelType                PixelType;
  typedef typename NumericTraits<PixelType>::RealType    RealType;
  typedef typename NumericTraits<PixelType>::ScalarRealType ScalarRealType;
  typedef typename TOutputImage::PixelType  OutputPixelType;

  /** Smart pointer typedef support.  */
  typedef typename TInputImage::Pointer  InputImagePointer;
  typedef typename TInputImage::ConstPointer  InputImageConstPointer;

  /** a type to represent the "kernel radius" */
  typedef typename itk::FixedArray<ScalarRealType, TInputImage::ImageDimension> RadiusType;

  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** Define the image type for internal computations
      RealType is usually 'double' in NumericTraits.
      Here we prefer float in order to save memory.  */


protected:
  ParabolicErodeImageFilter(){};
  virtual ~ParabolicErodeImageFilter() {};
//   void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  ParabolicErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  
};

} // end namespace itk

#endif
