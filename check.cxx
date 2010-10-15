#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryPerimeterEstimationCalculator.h"


int main(int argc, char * argv[])
{

  if( argc != 2 )
    {
    std::cerr << "usage: " << argv[0] << " " << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  typedef itk::BinaryPerimeterEstimationCalculator< IType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetImage( reader->GetOutput() );

  // itk::SimpleFilterWatcher watcher(filter, "filter");

  filter->Compute();
  std::cout << filter->GetPerimeter() << std::endl;

  return 0;
}

