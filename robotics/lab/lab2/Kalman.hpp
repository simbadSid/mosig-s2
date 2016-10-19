class Kalman{
    public:
		float mean;
		float std;
		float q;//noise associated to motion
		float r;//noise associated to observation
		float k;

		Kalman(float init_mean, float init_std, float init_q, float init_r);

		void prediction(int a);
		void estimation(int o);
};
