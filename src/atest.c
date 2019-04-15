#include "sapi.h"
#include "sapi_ultrasonic_hcsr04.h"
#include "sapi_datatypes.h"
#include <math.h>
/*
 *
 *
 *  evalTestPermissive  evaluate the param *pdst against *accepted by te following inequality
 *
 *         (-1) * (*accepted * 0.05 ) < *accepted - *pdst  < (*accepted * 0.05 )
 *
 * 			the inequalitty depicted above  describes an error of 0.05 of error tolerance related to the measure.
 *
 *
 *
 * */


bool_t evalTestPermissive(float *pdst, float *accepted){


	uint32_t param, acceptedD, diff;

return 0;

}

bool_t evalTest(uint32_t pdst, uint32_t accepted){

	uint32_t param, acceptedD, diff;
	param = pdst;
	acceptedD = accepted;
	diff = acceptedD - param;
	diff = fabs(diff);
	if(diff == 0){
		return 1;
	}else{
		return 0;
	}

}
