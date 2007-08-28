/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelPerimeterEstimationCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:30 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even 
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
    PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelPerimeterEstimationCalculator_txx
#define __itkLabelPerimeterEstimationCalculator_txx

#include "itkLabelPerimeterEstimationCalculator.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkSize.h"
#include "itkConnectedComponentAlgorithm.h"
#include <set>

namespace itk {

template <class TInputImage>
LabelPerimeterEstimationCalculator<TInputImage>
::LabelPerimeterEstimationCalculator()
{
  m_FullyConnected = false;
}



template<class TInputImage>
void
LabelPerimeterEstimationCalculator<TInputImage>
::Compute()
{
  
  m_Perimeters.clear();
  
  // ProgressReporter progress( this, 0, this->GetImage()->GetRequestedRegion().GetNumberOfPixels() );
  
  // reduce the region to avoid reading outside
  RegionType region = this->GetImage()->GetRequestedRegion();
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
  
  // to store the configurations count for all the labels
  typedef typename std::map< unsigned long, unsigned long > MapType;
  typedef typename std::map< InputImagePixelType, MapType > LabelMapType;
  LabelMapType confCount;

  for( iIt.GoToBegin(); !iIt.IsAtEnd(); ++iIt )
    {
    // 2 pass - find the labels in the neighborhood
    //        - count the configurations for all the labels

    typedef typename std::set< InputImagePixelType > LabelSetType;
    LabelSetType labelSet;
    for ( nIt= iIt.Begin();
      nIt != iIt.End();
      nIt++ )
      {
      labelSet.insert( nIt.Get() );
      }

    for( typename LabelSetType::const_iterator it=labelSet.begin();
      it!=labelSet.end();
      it++ )
      {

      unsigned long conf = 0;
      int i=0;

      for ( nIt= iIt.Begin();
        nIt != iIt.End();
        nIt++, i++ )
        {
        if( nIt.Get() == *it )
          {
          conf += 1 << i;
          }
        }

      confCount[ *it ][ conf ]++;

      }

    // progress.CompletedPixel();

    }

   for( typename LabelMapType::const_iterator it=confCount.begin();
     it!=confCount.end();
     it++ )
     {
     
     for( typename MapType::const_iterator it2=it->second.begin();
       it2!=it->second.end();
       it2++ )
       {
       // std::cout << it->first+0.0 << "  "  << it2->first << "  " << it2->second << std::endl;
       }
       
       m_Perimeters[ it->first ] = 1;
     }

}



template<class TInputImage>
void
LabelPerimeterEstimationCalculator<TInputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "FullyConnected: "  << m_FullyConnected << std::endl;
}
  
}// end namespace itk
#endif
