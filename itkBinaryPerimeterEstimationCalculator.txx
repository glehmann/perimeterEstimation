/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryPerimeterEstimationCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:30 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even 
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
    PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBinaryPerimeterEstimationCalculator_txx
#define __itkBinaryPerimeterEstimationCalculator_txx

#include "itkBinaryPerimeterEstimationCalculator.h"
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
BinaryPerimeterEstimationCalculator<TInputImage>
::BinaryPerimeterEstimationCalculator()
{
  m_FullyConnected = false;
  m_ForegroundValue = NumericTraits<InputImagePixelType>::max();
  m_Perimeter = 0;
}


template<class TInputImage>
void
BinaryPerimeterEstimationCalculator<TInputImage>
::Compute()
{  
  // reduce the region to avoid reading outside
  RegionType region = this->GetImage()->GetBufferedRegion();
  SizeType size = region.GetSize();
  for( int i=0; i<ImageDimension; i++ )
    {
    size[i]--;
    }
  region.SetSize( size );

  // the radius which will be used for all the shaped iterators
  Size< ImageDimension > radius;
  radius.Fill(1);

  // set up the iterator
  typedef ConstShapedNeighborhoodIterator<InputImageType> IteratorType;
  typename IteratorType::ConstIterator nIt;
  IteratorType iIt( radius, this->GetImage(), region );
//   ConstantBoundaryCondition<InputImageType> lcbc;
//   lcbc.SetConstant( NumericTraits<InputImagePixelType>::max() );
//   iIt.OverrideBoundaryCondition(&lcbc);
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
    // progress.CompletedPixel();
    }

   for( typename MapType::const_iterator it=confCount.begin();
     it!=confCount.end();
     it++ )
     {
     std::cout << it->first << "  " << it->second << std::endl;
     }

}



template<class TInputImage>
void
BinaryPerimeterEstimationCalculator<TInputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "FullyConnected: "  << m_FullyConnected << std::endl;
  os << indent << "ForegroundValue: "  << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_ForegroundValue) << std::endl;
  os << indent << "Perimeter: " << static_cast<typename NumericTraits< double >::PrintType>(m_Perimeter) << std::endl;
}
  
}// end namespace itk
#endif
