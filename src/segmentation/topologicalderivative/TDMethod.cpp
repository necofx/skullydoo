#include "TDMethod.h"

#include "common/Types.h"
#include "gui/ProgressWindowGUI.h"

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkStructuredPoints.h>

TDMethod::TDMethod():vtkSimpleImageToImageFilter(){
	distImage = vtkFloatArray::New();
	edgeImage = vtkFloatArray::New();
	rho=1;
	this->inputImage=NULL;
}

TDMethod::~TDMethod(){
	edgeImage->Delete();
	distImage->Delete();
}

void TDMethod::setClasses(ClassesVector cls){
	// first, delete the old list
	int comp = cls.size();
	this->classes.empty();
	classes = cls;
}

TDMethod::ClassesVector TDMethod::getClasses(){
	return classes;
}

void TDMethod::SimpleExecute(vtkImageData* input,vtkImageData* output){
	this->inputImage=input;
	inPtr = (float*)input->GetScalarPointer();
	outPtr = (float*)output->GetScalarPointer();
//	classes.empty();
//	classes.push_back(0);
//	classes.push_back(100);
//	rho = 0.98;
//	activeDirections[0]=activeDirections[1]=activeDirections[2]=1;

	input->GetDimensions(dims);
	input->GetScalarRange(scalarRange);

	imgSize = dims[0]*dims[1]*dims[2];
	if (classes.size()==0) {
		for (long int i = 0 ; i<imgSize ; i++){
			outPtr[i] = inPtr[i];
		}
		return;
	}
	normPtr = (float *)malloc(sizeof(float) * imgSize);

	//Allocate memory for the variations	
	edgeImage->SetNumberOfComponents(classes.size());
	edgeImage->SetNumberOfTuples(imgSize);
	edgeImage->Allocate(imgSize);

	distImage->SetNumberOfComponents(classes.size());
	distImage->SetNumberOfTuples(imgSize);
	distImage->Allocate(imgSize);

	// sort classes vector
	sort(classes.begin(),classes.end());
	//normalize classes values to [0,1].
	ClassesVector::iterator it;
	for (it = classes.begin(); it<=classes.end() ; it++){
		*it=(*it-scalarRange[0])/(scalarRange[1]-scalarRange[0]);
	}

	// initialize with the value that is in the middle of the list 
	int pos = classes.size() / 2;

	//normalize the input data to [0,1] and give initial value to outPtr.
	for (long int i = 0 ; i<imgSize ; i++){
		normPtr[i] = (inPtr[i] - scalarRange[0])/(scalarRange[1]-scalarRange[0]);
		outPtr[i] = classes[pos];
	}
	//here the algorithm actaly does his job
	UpdateProgress(0);
	iterativeProcess();
	
	//Get the values back to the original range
	for (i = 0 ; i<imgSize ; i++){
		outPtr[i] = (outPtr[i] * (scalarRange[1]-scalarRange[0]))-scalarRange[0];
	}

	this->inputImage = NULL;
	free(normPtr);
}

void TDMethod::iterativeProcess(){
	// Calculate the initial value for the cost function
	// because in the initial step every voxel is the same 
	// class we don't have to calculate the line integral.
	// CAUTION!!! this have to be alterated if we start from 
	// a better start point.
	calculateDistanceVariation(normPtr,outPtr,classes,distImage);

	//Calculate edges longitude
//	calculateEdgesVariation(normPtr,outPtr,classes,distImage,edgeImage);

	//Search for the minimum in tdImage
	//that means what is the class that's minimum for this voxel
	//in the cost function in this step. Then assign the class value 
	//to that voxel un the segmentation image.
//	minimazeCostFunction(outPtr,classes,edgeImage);
	minimazeCostFunction(outPtr,classes,distImage);
	float costFunctionAnt;
	costFunctionAnt=costFunctionMin-1;
	int iter=0;
	cout << "Iteration: "<< iter <<"\n   costFunctionMin: "<<costFunctionMin
		                         <<"\n   costFunctionAnt: "<<costFunctionAnt
								 <<"\n   totalVoxels: "    <<imgSize
								 <<"\n   changedVoxels: "  <<changedVoxels
								 <<"\n\n";
//	cout << "\nRho: "<<rho<<"\n";
	cout << "\n***Iterative process started***\n\n";
	//if the minimum is negative, we continue to iterate.
//	while (costFunctionMin > costFunctionAnt){
    while (iter<30){
		if (iter<10){
			UpdateProgress(iter/10);
		}else{
			UpdateProgress(iter/50);
		}
		costFunctionAnt=costFunctionMin;
		//Calculate distance
		calculateDistanceVariation(normPtr,outPtr,classes,distImage);
//		cout << "\n\nEn iterativeProcess costFunctionRange[0] vale: " << costFunctionRange[0] << "\n\n";

		//Calculate edges longitude
		calculateEdgesVariation(normPtr,outPtr,classes,distImage,edgeImage);

		//Search for the minimum in distImage
		//that means what is the class that's minimum for this voxel
		//in the cost function in this step. Then assign the class value
		//to that voxel un the segmentation image.
		minimazeCostFunction(outPtr,classes,edgeImage);
		iter++;
		cout << "Iteration: "<< iter <<"\n   costFunctionMin: "<<costFunctionMin<<"\n   costFunctionAnt: "<<costFunctionAnt<<"\n   changedVoxels: "<<changedVoxels<<"\n\n";
	}
	cout << "\n***Iterative process ended in "<< iter <<" iterations***\n";
}

void TDMethod::calculateEdgesVariation(float *normPtr, float *outPtr, ClassesVector classes, vtkFloatArray *distImage, vtkFloatArray *edgeImage){
	int ijk[3],ijkNeigs[6][3],nNeigs;
	float *neigValues,actValue,newDiff,actDiff,deriv;
	long int auxId,actId;
	nNeigs = (activeDirections[0]+activeDirections[1]+activeDirections[2])*2;
	neigValues = (float *) malloc(sizeof(float)* 6);
	costFunctionMin = edgeImage->GetDataTypeMax();
	for (ijk[2]=0;ijk[2]<dims[2];ijk[2]++){
		for (ijk[1]=0;ijk[1]<dims[1];ijk[1]++){
			for (ijk[0]=0;ijk[0]<dims[0];ijk[0]++){
				//ATENTION!!!!! depending on how am I going to treat limits
				//I have to initialize the neighbor values with different values.
				//Initialize all neighbors
				actId = inputImage->ComputePointId(ijk);
				actValue = outPtr[actId];
				int i;
				for (i=0;i<6;i++){
					neigValues[i]=actValue;
				}
				if (activeDirections[0]){
					//neighbor to the left
					ijkNeigs[0][0] = ijk[0]-1;
					ijkNeigs[0][1] = ijk[1];
					ijkNeigs[0][2] = ijk[2];
					
					//neighbor to the right
					ijkNeigs[1][0] = ijk[0]+1;
					ijkNeigs[1][1] = ijk[1];
					ijkNeigs[1][2] = ijk[2];

					//Get left neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[0]);
					//If it's inside the image get class value
					if (ijk[0]-1 >= 0){
						neigValues[0] = outPtr[auxId];
					}

					//Get right neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[1]);
					//If it's inside the image get class value
					if (ijk[0]+1 < dims[0] ){
						neigValues[1] = outPtr[auxId];
					}
				}
				if (activeDirections[1]){
					//neighbor to the back
					ijkNeigs[2][0] = ijk[0];
					ijkNeigs[2][1] = ijk[1]-1;
					ijkNeigs[2][2] = ijk[2];

					//neighbor to the front
					ijkNeigs[3][0] = ijk[0];
					ijkNeigs[3][1] = ijk[1]+1;
					ijkNeigs[3][2] = ijk[2];

					//Get back neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[2]);
					//If it's inside the image get class value
					if (ijk[1]-1 >= 0){
						neigValues[2] = outPtr[auxId];
					}

					//Get front neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[3]);
					//If it's inside the image get class value
					if (ijk[1]+1 < dims[1]){
						neigValues[3] = outPtr[auxId];
					}
				}
				if (activeDirections[2]){
					//neighbor to the top
					ijkNeigs[4][0] = ijk[0];
					ijkNeigs[4][1] = ijk[1];
					ijkNeigs[4][2] = ijk[2]-1;

					//neighbor to the bottom
					ijkNeigs[5][0] = ijk[0];
					ijkNeigs[5][1] = ijk[1];
					ijkNeigs[5][2] = ijk[2]+1;

					//Get top neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[4]);
					//If it's inside the image get class value
					if (ijk[2]-1 >= 0){
						neigValues[4] = outPtr[auxId];
					}

					//Get bottom neighbor id
					auxId = inputImage->ComputePointId(ijkNeigs[5]);
					//If it's inside the image get class value
					if (ijk[2]+1 < dims[2]){
						neigValues[5] = outPtr[auxId];
					}
				}
				int c,j;
				ClassesVector::iterator it;
				actDiff=0;
				for (j=0;j<6;j++){
					if (neigValues[j]!=actValue){
						actDiff++;
					}						
				}
				c = 0;
//				cout << "Para Id " << actId << "\n";
				for (it = classes.begin(); it < classes.end() ; it++){
					//For each neighbor, variate and count if is different
					newDiff=0;
					for (j=0;j<6;j++){
						(neigValues[j]!=*it)?newDiff++:NULL;
					}
					//Divide by 6 because I'm using normalized
					//values, we can control that with rho
					deriv = distImage->GetComponent(actId,c);
//					cout << "		cls: "<< *it << " derivAnt: " << deriv;
					//The signal is negative because instead of calculating
					//how many neighbors are the same, y calculate how many 
					//are different
					deriv += (1-rho) * ((newDiff-actDiff)/6) * 0.05;
//					cout << " derivAct: " << deriv << "\n";
					edgeImage->SetComponent(actId,c,deriv);
					c++;
				}
			}
		}
	}
	free(neigValues);
}

void TDMethod::minimazeCostFunction(float *outPtr, ClassesVector classes, vtkFloatArray *edgeImage){
	long int auxpid; //run the hole image
	float min,iterMin;
	float *comps = (float *)malloc(sizeof(float)*edgeImage->GetNumberOfComponents());
	float oldValue;
	changedVoxels = 0;
	edgeImage->GetTuple(0,comps);
	iterMin = comps[0];

//	search, on every voxel, the minimum component value.
//	loop over voxels
	for (auxpid=0 ; auxpid<imgSize ; auxpid++){
//		cout << "Estoy en auxpid: "<< auxpid << "\n";
//		get the next tuple
		edgeImage->GetTuple(auxpid,comps);
		min = comps[0];
		oldValue = outPtr[auxpid];
		outPtr[auxpid] = classes[0];
//		loop over components
		for (int i = 1 ; i < classes.size() ; i++){
//			is this component the minimum?
//			cout << " comps["<<i<<"] = "<<comps[i]<<" , ";
            if (comps[i] < min){ // && comps[i] < 0
				//Set that class value (the minimum one) to the voxel being processed.
				outPtr[auxpid] = classes[i];
				min = comps[i];
            }
//			cout << " outPtr["<<auxpid<<"] = "<<outPtr[auxpid]<<"\n";
		}
//		cout << " outPtr["<<auxpid<<"] = "<<outPtr[auxpid]<<"\n";
//		cout << " \n min = "<< min << "\n";
		if (outPtr[auxpid]!=oldValue) changedVoxels++;
		if (min<iterMin) {
			iterMin = min;
		}
	}
	if (iterMin<costFunctionMin) costFunctionMin = iterMin;
	free(comps);
//	cout << " \n costFunctionMin  vale = "<< costFunctionMin << "\n";
}

void TDMethod::calculateDistanceVariation(float *normPtr, float *outPtr, ClassesVector classes, vtkFloatArray *distImage){
	// Calculate the initial value for the cost function
	// because in the initial step every voxel is the same 
	// class we don't have to calculate the line integral.
	// CAUTION!!! this have to be alterated if we start from 
	// a better start point.
	long int ipid;
	float newCost,actCost,deriv;
	//Iterators to run the vector.
	ClassesVector::iterator clsIt;
	costFunctionMin = distImage->GetDataTypeMax();
	//For every point in the image
	for (ipid = 0 ; ipid<imgSize ; ipid++){
		//calculate the cost function value for the old (class) value
		//old = (normalizedInputImage[ipid]-outPtr[ipid])^2
		actCost = (normPtr[ipid]-outPtr[ipid]);
        actCost = actCost*actCost;
		int i;
		// for every class (TD image)
		for (clsIt=classes.begin(), i = 0; 
			clsIt < classes.end() ; i++ , clsIt++){
			//calculate the cost function this specific class.
			//deriv = (normalizedInputImage[ipid] - classes[clsIt])^2 - 
			//	      (normalizedInputImage[ipid] -   outPtr[ipid])^2

			newCost = (normPtr[ipid]-(*clsIt));
			newCost = newCost*newCost;
			//calculate the Topological derivative, for this pixel, for this test class
			deriv = rho * (newCost - actCost);
			distImage->SetComponent(ipid,i,deriv);
		}
	}
}

void TDMethod::setRho(float rho){
	this->rho = rho;
}

float TDMethod::getRho(){
	return rho;
}

void TDMethod::setActiveDirections(int *activeDirections){
	this->activeDirections[0] = activeDirections[0];
	this->activeDirections[1] = activeDirections[1];
	this->activeDirections[2] = activeDirections[2];
}
