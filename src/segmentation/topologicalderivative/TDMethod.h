#ifndef _TDMethod_h_
#define _TDMethod_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "common/VoxelModel.h"
#include <vtkImageData.h>
#include <vtkPointSet.h>
#include <vtkSimpleImageToImageFilter.h>
#include <vtkFloatArray.h>
#include <vector>
#include <algorithm>

class TDMethod:public vtkSimpleImageToImageFilter{
public:
	//The vector type must be the voxel type, in this case float.
	typedef std::vector<float> ClassesVector;

	TDMethod();
	virtual ~TDMethod();
	
	void setClasses(ClassesVector cls);
	ClassesVector getClasses();
	void setRho(float rho);
	float getRho();
	void setActiveDirections(int *activeDirections);

protected:
	//Contains the classes to segment the image in.
	ClassesVector classes;
	//Linear combination of the distance/line integral
	float rho;
	//Image size
	long int imgSize;
	//Contaisn 1's in the position of the active segmentation directions.
	int activeDirections[3];
	//Input image Dimensions
	int dims[3];
	long changedVoxels;

	//Contains pointer to image input. ONLY VALID DURING SIMPLE EXECUTION!!!
	vtkImageData* inputImage;
	//Contains the input image data.
	float* inPtr;
	//Contains the normalized input image data.
	float* normPtr;
	//Contains the output(segmented) image data.
	float* outPtr;
	vtkFloatArray* distImage;
	//Contains a vtkFloatArray for every class.
	vtkFloatArray* edgeImage;
	//Original Image values range
	//double costFunctionRange[2];
	float costFunctionMin;
	float scalarRange[2];

	void minimazeCostFunction(float *outPtr, ClassesVector classes, vtkFloatArray *derivImage);
	virtual void iterativeProcess();

	void calculateDistanceVariation(float *normPtr, float *outPtr, ClassesVector classes, vtkFloatArray *derivImage);
	void calculateEdgesVariation(float *normPtr, float *outPtr, ClassesVector classes, vtkFloatArray *distImage, vtkFloatArray *edgeImage);

	virtual void SimpleExecute(vtkImageData* input,vtkImageData* output);
};

#endif