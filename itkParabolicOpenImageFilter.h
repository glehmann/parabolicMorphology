#ifndef __itkParabolicOpenImageFilter_h
#define __itkParabolicOpenImageFilter_h

#include "itkParabolicOpenCloseImageFilter.h"
#include "itkCropImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkStatisticsImageFilter.h"

/* this class implements padding and cropping, so we don't just
* inherit from the OpenCloseImageFitler */

namespace itk
{
template <typename TInputImage,
          typename TOutputImage= TInputImage >
class ITK_EXPORT ParabolicOpenImageFilter:
    public ImageToImageFilter<TInputImage,
			      TOutputImage>
{

public:
  /** Standard class typedefs. */
  typedef ParabolicOpenImageFilter  Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);


  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef TOutputImage                                   OutputImageType;
  typedef typename TInputImage::PixelType                InputPixelType;
  typedef typename NumericTraits<InputPixelType>::RealType    RealType;
  typedef typename NumericTraits<InputPixelType>::ScalarRealType ScalarRealType;
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

  // set all of the scales the same
  void SetScale(ScalarRealType scale)
  {
    this->m_MorphFilt->SetScale(scale);
  }
  // different scale for each direction
  void SetScale(RadiusType scale)
  {
    this->m_MorphFilt->SetScale(scale);
  }

  void SetUseImageSpacing(bool B)
  {
    this->m_MorphFilt->SetUseImageSpacing(B);
  }

  itkSetMacro(SafeBorder, bool);
  itkGetConstReferenceMacro(SafeBorder, bool);
  // should add the Get methods

  
protected:
  void GenerateData();
  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef ParabolicOpenCloseImageFilter<TInputImage, true, TOutputImage> MorphFilterType;
  typedef ConstantPadImageFilter<TInputImage, TInputImage> PadFilterType;
  typedef CropImageFilter<TOutputImage, TOutputImage> CropFilterType;
  typedef StatisticsImageFilter<InputImageType> StatsFilterType;

  ParabolicOpenImageFilter()
  {
    m_MorphFilt = MorphFilterType::New();
    m_PadFilt = PadFilterType::New();
    m_CropFilt = CropFilterType::New();
    m_StatsFilt = StatsFilterType::New();
    m_SafeBorder = true;
  }
  virtual ~ParabolicOpenImageFilter() {};
  
  
private:
  ParabolicOpenImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename MorphFilterType::Pointer m_MorphFilt;
  typename PadFilterType::Pointer m_PadFilt;
  typename CropFilterType::Pointer m_CropFilt;
  typename StatsFilterType::Pointer m_StatsFilt;
  bool m_SafeBorder;
};

} // end namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkParabolicOpenImageFilter.txx"
#endif

#endif

