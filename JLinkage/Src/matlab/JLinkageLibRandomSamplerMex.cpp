// JLinkageLibRandomSamplerMex.cpp : mex-function interface implentation file

#include "RandomSampler.h"
#include "JLinkage.h"
#include "PrimitiveFunctions.h"
#include "../../VPPrimitive.h" //FC
#include "mex.h"

// Create the waitbar
mxArray *hw,*title,*percent;
double *percentDoublePtr;
mxArray *input_array[3];
mxArray *output_array[1];
time_t initialTime, currentTime;
char tempString[128];


void InitializeWaitbar(){
	// Initialize waitbar
	title = mxCreateString("In progress ...");
	percent = mxCreateDoubleMatrix(1,1, mxREAL);
	percentDoublePtr = (double *)mxGetPr(percent);
	*percentDoublePtr = 0.0;

	input_array[0] = percent;
	input_array[1] = title;

	mexCallMATLAB(1, output_array, 2, input_array, "waitbar");
	hw = output_array[0];
	input_array[1] = hw;
	initialTime = time(NULL);
}

void UpdateWaitbar(float nValue){
	// update the waitbar
	
	mexCallMATLAB(0, output_array, 2, input_array, "waitbar");

	static float nPreviousValue = 0.0f;

			
	if( (int)(floor(nValue * 100.f)) - (int) (floor(nPreviousValue * 100.f)) > 0){
		currentTime = time(NULL);
		if((currentTime - initialTime) == 0){
			nPreviousValue = nValue;
			return;
		}
		
		int estimetedTimeInSeconds = 0;
		if(nValue > 0.0)
			estimetedTimeInSeconds =(int)( ((float)(currentTime - initialTime)  * (float)((1.f-nValue) * 100.f)) /(float)((nValue) * 100.f)) ;
		
		int hours = (estimetedTimeInSeconds) / (3600);
		int minutes = ((estimetedTimeInSeconds) / 60) % 60;
		int seconds = (estimetedTimeInSeconds) % 60;
		
		
		*percentDoublePtr = nValue;
		mxDestroyArray(title);
		sprintf(tempString, "In progress ... (Estimated time = %d H : %d m : %d s)",hours, minutes,seconds);
		title = mxCreateString(tempString );
		input_array[0] = percent;
		input_array[1] = hw;		
		input_array[2] = title;
		mexCallMATLAB(0, output_array, 3, input_array, "waitbar");
	}

	nPreviousValue = nValue;
}

void PrintInMatlab(const char *nStr){
	mexPrintf(nStr);
	mexEvalString("drawnow");
}

void CloseWaitbar(){
	// Close the waitbar
	input_array[0] = hw;
	mexCallMATLAB(0, output_array, 1, input_array, "close");
	mxDestroyArray(percent);
	mxDestroyArray(title);
}

void PrintHelp(){

	PrintInMatlab("\n\n *** JLinkageRandomSamplerMex v.1.0 *** Part of the SamantHa Project");
	PrintInMatlab("\n     Author roberto.toldo@univr.it - Vips Lab - Department of Computer Science - University of Verona(Italy)");
	PrintInMatlab("\n     vpdetection Author Chen Feng - simbaforrest@gmail.com - University of Michigan");
	PrintInMatlab("\n ***********************************************************************");
	PrintInMatlab("\n Usage: [Labels] = JLnkRandomSampler(Points, NSamples, ModelType, (FirstSamplingVectorProb = []), (SamplingType = UNIFORM), (Par1), (Par2), (Par3))");
	PrintInMatlab("\n Input:");
	PrintInMatlab("\n        Points - Input dataset (Dimension x NumberOfPoints)");
	PrintInMatlab("\n        NSamples - Number of desired samples");
	PrintInMatlab("\n        ModelType - type of models extracted. Currently the model supported are: 0 - Planes 1 - 2dLines 2 - Vanishing Points");
	PrintInMatlab("\n        FirstSamplingVectorProb(facultative) - Associate to each point a (non-uniform) probability to be randomly picked (leave [] to set a uniform probability for each point)");
	PrintInMatlab("\n        SamplingType(facultative) - Non first sampling strategy: 0 - Uniform(default) 1 - Exp 2 - Kd-Tree 3 - Memory Efficient Kd-Tree(Slower)");
	PrintInMatlab("\n        Par1(facultative) - Sigma Exp(default = 1.0) or Neighbor search for Kd-Tree (default = 10)");
	PrintInMatlab("\n        Par2(facultative) - only for kd-tree non first sampling: close points probability (default = 0.8)");
	PrintInMatlab("\n        Par3(facultative) - only for kd-tree non first sampling: far points probability (default = 0.2)");
	PrintInMatlab("\n Output:");
	PrintInMatlab("\n        Models - Generated hypotesis(Dimension x NSampler)");
	PrintInMatlab("\n");

	
}

enum MODELTYPE{
	MT_PLANE,
	MT_LINE,
	MT_VP, //FC
	MT_SIZE
};


double *mTempMPointer = NULL;
//// Input arguments
// Arg 0, points
std::vector<std::vector<float> *> *mDataPoints;
// Arg 1, Number of desired samples
unsigned int mNSample;
// Arg 2, type of model: 0 - Planes 1 - 2dLines
MODELTYPE mModelType;
// ----- facultatives
// Arg 3, Non uniform first sampling vector(NULL-empty if uniform sampling is choosen)
double *mFirstSamplingVector = NULL;
// Arg 4, Non first sampling type: 0 - Uniform(def) 1 - Exp 2 - Kd-Tree
RS_NFSAMPLINGTYPE mNFSamplingType = NFST_UNIFORM;
// Arg 5, Sigma Exp(def = 1.0) or neigh search for Kd-Tree (def = 10)
double mSigmaExp = 1.0; int mKdTreeRange = 10;
// Arg 6, only for kd-tree non first sampling: close points probability (def = 0.8)
double mKdTreeCloseProb = 0.8;
// Arg 7, only for kd-tree non first sampling: far points probability (def = 0.2)
double mKdTreeFarProb = 0.2;

unsigned int mMSS = 0;

// Function pointers
std::vector<float>  *(*mGetFunction)(const std::vector<sPt *> &nDataPtXMss, const std::vector<unsigned int>  &nSelectedPts);
float (*mDistanceFunction)(const std::vector<float>  &nModel, const std::vector<float>  &nDataPt);


//// Output arguments
// Arg 0, NPoints x NSample logical preference set matrix
bool *mPSMatrix;

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{ 

	
	/* retrive arguments */
	if( nrhs < 3 ) {
		PrintHelp();
		mexErrMsgTxt("At least 3 arguments are required"); 
	}
	if( nlhs != 1 ) {
		PrintHelp();
		mexErrMsgTxt("1 output required."); 
	}
	
	// arg2 : NSample
	mTempMPointer = mxGetPr(prhs[1]);
	mNSample = (unsigned int) *mTempMPointer;
	if(mNSample == 0)
		mexErrMsgTxt("Invalid NSample");

	// arg3 : modelType;
	mTempMPointer = mxGetPr(prhs[2]);
	mModelType = (MODELTYPE)(unsigned int) *mTempMPointer;
	switch(mModelType){
		case MT_PLANE: mMSS = 3; break;
		case MT_LINE: mMSS = 2; break;
		case MT_VP: mMSS = 2; break; //FC
		default: mexErrMsgTxt("Invalid model type"); break;
	}

	

	// Arg 0, data points	
	mTempMPointer = mxGetPr(prhs[0]);
	
	if( mxGetN(prhs[0]) < mMSS ||
		(mModelType == MT_PLANE && (mxGetM(prhs[0]) != 3 || mMSS != 3)))
		mexErrMsgTxt("Invalid data points vector");
	
	// Build mDataPointVector
	mDataPoints = new std::vector< std::vector<float> *>(mxGetN(prhs[0]));
	for(unsigned int i=0; i<mxGetN(prhs[0]); i++){
		(*mDataPoints)[i] = new std::vector<float>(mxGetM(prhs[0]));
		for(unsigned int j=0; j<mxGetM(prhs[0]) ; j++)
			(*(*mDataPoints)[i])[j] = (float)mTempMPointer[(i*mxGetM(prhs[0])) + j];
	}


	// Arg3: non uniform sampling
	if(nrhs > 4 && mxGetN(prhs[3]) > 0 && mxGetM(prhs[3]) > 0){
		mFirstSamplingVector = mxGetPr(prhs[3]);
		if((mxGetN(prhs[3]) != mDataPoints->size() && mxGetM(prhs[3]) == 1) || 
			(mxGetM(prhs[3]) != mDataPoints->size() && mxGetN(prhs[3]) == 1))

			mexErrMsgTxt("Invalid first sampling vector");
	}
	// Null pointer otherwise

	// Arg 4, Non first sampling type: 0 - Uniform(def) 1 - Exp 2 - Kd-Tree 3 - Memory efficient Kd-Tree
	if(nrhs > 4){
		mTempMPointer = mxGetPr(prhs[4]);
		if((RS_NFSAMPLINGTYPE)(unsigned int) *mTempMPointer >= NFST_SIZE)
			mexErrMsgTxt("Invalid Non first sampling type");
		mNFSamplingType = (RS_NFSAMPLINGTYPE)(unsigned int) *mTempMPointer;
	}


	// Arg 5, Sigma Exp(def = 1.0) or Range search for Kd-Tree (def = 1.0)
	if(nrhs > 5){
		mTempMPointer = mxGetPr(prhs[5]);
		if(NFST_EXP == mNFSamplingType)
			mSigmaExp = *mTempMPointer;
		if(NFST_NN == mNFSamplingType || NFST_NN_ME == mNFSamplingType )
			mKdTreeRange = (int)*mTempMPointer;
		if(mKdTreeRange <= 0 || mSigmaExp < 0.0)
			mexErrMsgTxt("Invalid Sigma exp or KdTreeRange");
	}

	double mKdTreeCloseProb = 1.0;
	double mKdTreeFarProb = 1.0;

	// Arg 6, only for kd-tree non first sampling: close points probability (def = 1.0)
	if(nrhs > 6){
		mTempMPointer = mxGetPr(prhs[6]);
		mKdTreeCloseProb = *mTempMPointer;

		if(mKdTreeCloseProb < 0.0 )
			mexErrMsgTxt("Invalid mKdTreeCloseProb");
	}

	// Arg 7, only for kd-tree non first sampling: far points probability (def = 2.0)
	if(nrhs > 7){
		mTempMPointer = mxGetPr(prhs[7]);
		mKdTreeFarProb = *mTempMPointer;

		if(mKdTreeCloseProb < 0.0 )
			mexErrMsgTxt("Invalid mKdTreeFarProb");
	}


	// Set the distance and get functions
	if(mModelType == MT_PLANE){
		mGetFunction = GetFunction_Plane;
		mDistanceFunction = DistanceFunction_Plane;
	}
	if(mModelType == MT_LINE){
		mGetFunction = GetFunction_Line;
		mDistanceFunction = DistanceFunction_Line;
	}
	//FC
	if(mModelType == MT_VP){
		mGetFunction = GetFunction_VP;
		mDistanceFunction = DistanceFunction_VP;
	}

	
	RandomSampler mRandomSampler(mGetFunction, mDistanceFunction,(int)(*(*mDataPoints)[0]).size()-1, mMSS, (int)mDataPoints->size(),true);

	PrintInMatlab("Inizialing Data \n");
	PrintInMatlab("\t Loading Points... \n");
	mRandomSampler.SetPoints(mDataPoints);

	PrintInMatlab("\t Inizialing Probabilities... \n");
	if(mFirstSamplingVector != NULL)
		for(unsigned int i=0; i < mDataPoints->size(); i++)
			mRandomSampler.SetFirstSamplingProb(i, (float)mFirstSamplingVector[i]);

	if(NFST_EXP == mNFSamplingType)
		mRandomSampler.SetNFSamplingTypeExp((float)mSigmaExp);

	if(NFST_NN == mNFSamplingType)
		mRandomSampler.SetNFSamplingTypeNN(mKdTreeRange, (float)mKdTreeCloseProb, (float)mKdTreeFarProb, false);
	if(NFST_NN_ME == mNFSamplingType)
		mRandomSampler.SetNFSamplingTypeNN(mKdTreeRange, (float)mKdTreeCloseProb, (float)mKdTreeFarProb, true);
	
	PrintInMatlab("Generating Hypotesis \n");
	InitializeWaitbar();
	std::vector<std::vector<float> *> *mModels = mRandomSampler.GetNSample(mNSample, 0, NULL, &UpdateWaitbar);
	CloseWaitbar();

	// First ouput : Model list

	plhs[0] = mxCreateDoubleMatrix(((*mModels)[0])->size(), mNSample, mxREAL); 
	double *mTempMPointerDouble = (double *)mxGetPr(plhs[0]);
	for(unsigned int i = 0; i < mNSample; i++)
		for(unsigned int j = 0; j < ((*mModels)[i])->size(); j++)
			mTempMPointerDouble[i * ((*mModels)[0])->size() + j] = (double)(*((*mModels)[i]))[j]; 
	

	for(unsigned int i=0; i<mDataPoints->size(); i++)
		delete (*mDataPoints)[i];
	delete mDataPoints;

	for(unsigned int i=0; i < mModels->size(); i++)
		delete (*mModels)[i];
	delete mModels;


    return;

}


