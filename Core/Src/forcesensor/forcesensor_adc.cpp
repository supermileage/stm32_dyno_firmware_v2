#include <forcesensor/forcesensor_adc.h>

#define SIXTEEN_BIT_MAX 65535
#define MAX_FORCE 25
#define LBF_TO_NEWTON 4.44822

uint16_t adcValue = 0;
volatile bool timerflag = false;

class forcesensorADC /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		forcesensorADC(TIM_HandleTypeDef* timer, osMessageQueueId_t osHandle, ADC_HandleTypeDef* adcHandle);
		virtual ~forcesensorADC() = default;

		bool Init();
		void Run();
		void Toggle(bool status,
		                force_sensor_adc_to_session_controller msg,
		                osMessageQueueId_t osHandle);

	private:
		float GetForce(void);

		TIM_HandleTypeDef* _timer;
		osMessageQueueId_t _osHandle;
		ADC_HandleTypeDef* _adcHandle;
		force_sensor_adc_to_session_controller _msg; // object of force_sensor_adc_to_session_controller
		bool _adc_toggle;

};

forcesensorADC::forcesensorADC(TIM_HandleTypeDef* timer, osMessageQueueId_t osHandle, ADC_HandleTypeDef* adcHandle) : /* Constructor */
		_timer(timer),
		_osHandle(osHandle),
		_adcHandle(adcHandle),
		_msg{},
		_adc_toggle{}
{}

void forcesensorADC::Toggle(bool status, force_sensor_adc_to_session_controller msg, osMessageQueueId_t osHandle)
{
    bool confirm = false; // Will likely be changed by reading message

    if (status && !confirm)
        _adc_toggle = true;
    else
        _adc_toggle = false;
}

bool forcesensorADC::Init()
{
	return true;
}

void forcesensorADC::Run(void)
{

	timerflag = false;
	HAL_ADC_Start_IT(_adcHandle); // starts adc, and does callback once fired
	while (!timerflag); // puts "Run" on hold until callback occurs

	_msg.adc_timestamp = 0;
	_msg.adc_force_action = GetForce();
//	osStatus_t status = osMessageQueuePut(_osHandle, &_msg, 0, 0);

	osStatus_t status;
	bool enableADC = false;

	while (1)
	{
	    status = osMessageQueueGet(_osHandle, &enableADC, NULL, 0);
	    if (status == osOK)
	    {
	        Toggle(enableADC, _msg, _osHandle);
	    }

	    if (enableADC)
	    {
	        float value = static_cast<float>(adcValue);
	        osMessageQueuePut(_osHandle, &value, 0, 0);
	    }
	}

}



float forcesensorADC::GetForce(void)
{
	return static_cast<float> (adcValue) / SIXTEEN_BIT_MAX * MAX_FORCE * LBF_TO_NEWTON; // Have the calculation here
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC2)
    {
        adcValue = HAL_ADC_GetValue(hadc); // read converted value
        timerflag = true;
    }
}

extern "C" void force_sensor_adc_main(TIM_HandleTypeDef* timer, osMessageQueueId_t osHandle, ADC_HandleTypeDef* adcHandle)
{
	forcesensorADC forcesensor = forcesensorADC(timer, osHandle, adcHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
