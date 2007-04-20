/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryPerimeterEstimationImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:30 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even 
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
    PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBinaryPerimeterEstimationImageFilter_txx
#define __itkBinaryPerimeterEstimationImageFilter_txx

#include "itkBinaryPerimeterEstimationImageFilter.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkSize.h"
#include "itkConnectedComponentAlgorithm.h"

namespace itk {

template <class TInputImage>
BinaryPerimeterEstimationImageFilter<TInputImage>
::BinaryPerimeterEstimationImageFilter()
{
  m_FullyConnected = false;
  m_ForegroundValue = NumericTraits<InputImagePixelType>::max();
}


template <class TInputImage>
void 
BinaryPerimeterEstimationImageFilter<TInputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the inputs
  InputImagePointer  inputPtr = 
    const_cast< InputImageType * >( this->GetInput() );
  
  if ( !inputPtr )
    { return; }

  // We need to
  // configure the inputs such that all the data is available.
  //
  inputPtr->SetRequestedRegion(inputPtr->GetLargestPossibleRegion());
}


template <class TInputImage>
void 
BinaryPerimeterEstimationImageFilter<TInputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}


template<class TInputImage>
void
BinaryPerimeterEstimationImageFilter<TInputImage>
::GenerateData()
{
  this->AllocateOutputs();
  ProgressReporter progress( this, 0, this->GetOutput()->GetRequestedRegion().GetNumberOfPixels() );
  
  // the radius which will be used for all the shaped iterators
  Size< ImageDimension > radius;
  radius.Fill(1);

  // set up the iterator
  typedef ConstShapedNeighborhoodIterator<InputImageType> IteratorType;
  typename IteratorType::ConstIterator nIt;
  IteratorType iIt( radius, this->GetInput(), this->GetOutput()->GetRequestedRegion() );
  ConstantBoundaryCondition<InputImageType> lcbc;
  lcbc.SetConstant( NumericTraits<InputImagePixelType>::max() );
  iIt.OverrideBoundaryCondition(&lcbc);
  // we want to search the neighbors with offset >= 0
  // 2D -> 4 neighbors
  // 3D -> 8 neighbors
  typename IteratorType::OffsetType offset;
  unsigned int centerIndex = iIt.GetCenterNeighborhoodIndex();
  for( unsigned int d=centerIndex+1; d < 2*centerIndex+1; d++ )
    {
    offset = iIt.GetOffset( d );
    iIt.ActivateOffset( offset );
    for ( int j=0; j<ImageDimension; j++ )
      {
      if( offset[j] < 0 )
        {
        iIt.DeactivateOffset( offset );
        }
      }
    }
  
  // to store the configurations count
  typedef typename std::map< unsigned long, unsigned long > MapType;
  MapType confCount;

  for( iIt.GoToBegin(); !iIt.IsAtEnd(); ++iIt )
    {
    unsigned long conf = 0;
    int i=0;
    for ( nIt= iIt.Begin();
      nIt != iIt.End();
      nIt++, i++ )
      {
      if( nIt.Get() == m_ForegroundValue )
        {
        conf += 1 << i;
        }
      }
    confCount[ conf ]++;
    progress.CompletedPixel();
    }

//   for( typename MapType::const_iterator it=confCount.begin();
//     it!=confCount.end();
//     it++ )
//     {
//     std::cout << it->first << "  " << it->second << std::endl;
//     }

}



template<class TInputImage>
void
BinaryPerimeterEstimationImageFilter<TInputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "FullyConnected: "  << m_FullyConnected << std::endl;
  os << indent << "ForegroundValue: "  << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_ForegroundValue) << std::endl;
}
  
}// end namespace itk
#endif
