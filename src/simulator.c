#include <stdio.h>

// model specific constants
# define GUID "{8f600e4d-a6f4-c2ed-3558-a9cbedce34b9}"
#ifndef FMI2_FUNCTION_PREFIX
#define FMI2_FUNCTION_PREFIX Addition_
#endif

// no runtime resources
#define RESOURCE_LOCATION "file:///C:/Users/schyan01/git/fmu_addition" // absolut path to the unziped fmu
#include "fmi2Functions.h"

// callback functions
static void cb_logMessage(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...) {
	printf("%s\n", message);
}

static void* cb_allocateMemory(size_t nobj, size_t size) {
	return calloc(nobj, size);
}

static void cb_freeMemory(void* obj) {
	free(obj);
}

#define CHECK_STATUS(S) { status = S; if (status != fmi2OK) goto TERMINATE; }

int main(int argc, char *argv[]) {

	fmi2Status status = fmi2OK;

	fmi2CallbackFunctions callbacks = {cb_logMessage, cb_allocateMemory, cb_freeMemory, NULL, NULL};
	
	fmi2Component c = Addition_fmi2Instantiate("instance1", fmi2CoSimulation, GUID, RESOURCE_LOCATION, &callbacks, fmi2False, fmi2False);
	
	if (!c) return 1;

	fmi2Real Time = 0;
	fmi2Real stepSize = 1;
	fmi2Real stopTime = 10;

	// Informs the FMU to setup the experiment. Must be called after fmi2Instantiate and befor fmi2EnterInitializationMode
	CHECK_STATUS(Addition_fmi2SetupExperiment(c, fmi2False, 0, Time, fmi2False, 0));
	
	// Informs the FMU to enter Initialization Mode.
	CHECK_STATUS(Addition_fmi2EnterInitializationMode(c));

	fmi2ValueReference x_ref = 0;
	fmi2Real x = 0;

	fmi2ValueReference y_ref = 1;
	fmi2Real y = 0;

	fmi2ValueReference Ergebnis_ref = 2;
	fmi2Real Ergebnis;

	CHECK_STATUS(Addition_fmi2SetReal(c, &x_ref, 1, &x));
	CHECK_STATUS(Addition_fmi2SetReal(c, &y_ref, 1, &y));

	CHECK_STATUS(Addition_fmi2ExitInitializationMode(c));

	printf("time, x, y, Ergenbis\n");

	for (int nSteps = 0; nSteps <= 10; nSteps++) {

		Time = nSteps * stepSize;

		// set an input
		CHECK_STATUS(Addition_fmi2SetReal(c, &x_ref, 1, &x));
		CHECK_STATUS(Addition_fmi2SetReal(c, &y_ref, 1, &y));

		// perform a simulation step
		CHECK_STATUS(Addition_fmi2DoStep(c, Time, stepSize, fmi2True));	//The computation of a time step is started.
		
		// get an output
		CHECK_STATUS(Addition_fmi2GetReal(c, &Ergebnis_ref, 1, &Ergebnis));

		printf("%.2f, %.0f, %.0f, %.0f\n", Time, x, y, Ergebnis);

		x++;
		y+=2;
	}

TERMINATE:

	Addition_fmi2Terminate(c);

	// clean up
	if (status < fmi2Fatal) {
		Addition_fmi2FreeInstance(c);
	}
	
	return status;
}
