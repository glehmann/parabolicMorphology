#ifndef __itkParabolicOpenCloseImageFilter_txx
#define __itkParabolicOpenCloseImageFilter_txx

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "itkParabolicOpenCloseImageFilter.h"
//#define NOINDEX
#ifndef NOINDEX
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#else
#include "itkImageLinearIterator.h"
#include "itkImageLinearConstIterator.h"
#endif
#include "itkStatisticsImageFilter.h"
#include "itkParabolicMorphUtils.h"

namespace itk
{

template <typename TInputImage, bool doOpen,  typename TOutputImage>
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::ParabolicOpenCloseImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfRequiredInputs( 1 );
// needs to be selected according to erosion/dilation
  if (doOpen)
    {
    // erosion then dilation
    m_Extreme1 = NumericTraits<PixelType>::max();
    m_Extreme2 = NumericTraits<PixelType>::min();
    m_MagnitudeSign1 = -1;
    m_MagnitudeSign2 = 1;
    } 
  else
    {
    // dilation then erosion
    m_Extreme1 = NumericTraits<PixelType>::min();
    m_Extreme2 = NumericTraits<PixelType>::max();
    m_MagnitudeSign1 = 1;
    m_MagnitudeSign2 = -1;
    }
  m_Extreme = m_Extreme1;
  m_MagnitudeSign = m_MagnitudeSign1;
  m_UseImageSpacing = false;
}

template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::SetScale( ScalarRealType scale )
{
  RadiusType s;
  s.Fill(scale);
  this->SetScale( s );
}

template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  if( image )
    {
    image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
    }
}

template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage,doOpen, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if (out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}


template <typename TInputImage, bool doOpen,  typename TOutputImage >
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage >
::GenerateData(void)
{

#ifndef NOINDEX
  typedef ImageLinearConstIteratorWithIndex< TInputImage  >  InputConstIteratorType;
  typedef ImageLinearIteratorWithIndex< TOutputImage >  OutputIteratorType;

  // for stages after the first
  typedef ImageLinearConstIteratorWithIndex< TOutputImage  >  OutputConstIteratorType;
#else
  typedef ImageLinearConstIterator< TInputImage  >  InputConstIteratorType;
  typedef ImageLinearIterator< TOutputImage >  OutputIteratorType;

  // for stages after the first
  typedef ImageLinearConstIterator< TOutputImage  >  OutputConstIteratorType;
#endif

  typedef ImageRegion< TInputImage::ImageDimension > RegionType;

  typename TInputImage::ConstPointer   inputImage(    this->GetInput ()   );
  typename TOutputImage::Pointer       outputImage(   this->GetOutput()   );


  const unsigned int imageDimension = inputImage->GetImageDimension();

  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  RegionType region = inputImage->GetRequestedRegion();


  InputConstIteratorType  inputIterator(  inputImage,  region );
  OutputIteratorType      outputIterator( outputImage, region );
  OutputConstIteratorType inputIteratorStage2( outputImage, region );

  // setup the progress reporting 
  unsigned int numberOfLinesToProcess = 0;
  for (unsigned  dd = 0; dd < imageDimension; dd++)
    {
    numberOfLinesToProcess += region.GetSize()[dd];
    }
  numberOfLinesToProcess *= 2;

  ProgressReporter progress(this,0, numberOfLinesToProcess);

  // deal with the first dimension - this should be copied to the
  // output if the scale is 0
  if (m_Scale[0] > 0)
    {
    // Perform as normal
//     RealType magnitude = 1.0/(2.0 * m_Scale[0]);
    unsigned long LineLength = region.GetSize()[0];
    RealType image_scale = this->GetInput()->GetSpacing()[0];

    doOneDimension<InputConstIteratorType,OutputIteratorType, 
      RealType, OutputPixelType, !doOpen>(inputIterator, outputIterator, 
					  progress, LineLength, 0, 
					  this->m_MagnitudeSign, 
					  this->m_UseImageSpacing,
					  this->m_Extreme,
					  image_scale, 
					  this->m_Scale[0]);
    }
  else 
    {
    // copy to output
    typedef ImageRegionConstIterator<TInputImage> InItType;
    typedef ImageRegionIterator<TOutputImage> OutItType;

    InItType InIt(inputImage, region);
    OutItType OutIt(outputImage, region);
    while (!InIt.IsAtEnd())
      {
      OutIt.Set(static_cast<OutputPixelType>(InIt.Get()));
      ++InIt;
      ++OutIt;
      }

    }
  // now deal with the other dimensions for first stage
  for (unsigned dd = 1; dd < imageDimension; dd++)
    {
    // Perform as normal
//     RealType magnitude = 1.0/(2.0 * m_Scale[dd]);
    unsigned long LineLength = region.GetSize()[dd];
    RealType image_scale = this->GetInput()->GetSpacing()[dd];
    
    doOneDimension<OutputConstIteratorType,OutputIteratorType, 
      RealType, OutputPixelType, !doOpen>(inputIteratorStage2, outputIterator, 
					  progress, LineLength, dd, 
					  this->m_MagnitudeSign, 
					  this->m_UseImageSpacing,
					  this->m_Extreme,
					  image_scale, 
					  this->m_Scale[dd]);
      
    }
  // swap over the parameters controlling erosion/dilation
  m_Extreme = m_Extreme2;
  m_MagnitudeSign = m_MagnitudeSign2;

  // now deal with the other dimensions for second stage
  for (unsigned dd = 0; dd < imageDimension; dd++)
    {
    if (m_Scale[dd] > 0)
      {
      //RealType magnitude = 1.0/(2.0 * m_Scale[dd]);
      unsigned long LineLength = region.GetSize()[dd];
      RealType image_scale = this->GetInput()->GetSpacing()[dd];
      
      doOneDimension<OutputConstIteratorType,OutputIteratorType,
 	RealType, OutputPixelType, doOpen>(inputIteratorStage2, outputIterator, 
					   progress, LineLength, dd, 
					   this->m_MagnitudeSign, 
					   this->m_UseImageSpacing,
					   this->m_Extreme,
					   image_scale, 
					   this->m_Scale[dd]);
      }
    }
  // swap them back
  m_Extreme = m_Extreme1;
  m_MagnitudeSign = m_MagnitudeSign1;
}

template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  if (m_UseImageSpacing)
    {
    os << "Scale in world units: " << m_Scale << std::endl;
    }
  else
    {
    os << "Scale in voxels: " << m_Scale << std::endl;
    }
}


} // namespace itk
#endif
