// --- Pre-calculated Gains (Replace with your actual offline tuning values) ---
const float K[4]     = {15.2, 1.5, 0.8, 0.9}; // State Feedback Gains (Pitch, PitchRate, Pos, Vel)
const float K_int    = 5.0;                   // Integral Gain
const float h        = 0.01;                  // Loop time (e.g., 100Hz = 0.01s)

// --- Limits ---
const float MAX_VOLTAGE = 9.0;                // 9V Battery limit
const float MAX_INTEGRAL = 2.0;               // Anti-windup cap to prevent math overflow

// --- Global Integral Memory ---
float x_int = 0.0;                            

// --- Main Control Function ---
void updateController(float est_pitch, float est_pitch_rate, float est_pos, float est_velocity) {
    
    // 1. Pack the estimated states into an array
    float x_hat[4] = {est_pitch, est_pitch_rate, est_pos, est_velocity};
    
    // 2. Update the Integral State (Summing the pitch angle over time)
    x_int = x_int + (est_pitch * h);
    
    // 3. Anti-Windup Safeguard (Caps the memory if held sideways)
    if (x_int > MAX_INTEGRAL)  x_int = MAX_INTEGRAL;
    if (x_int < -MAX_INTEGRAL) x_int = -MAX_INTEGRAL;

    // 4. Calculate base LQG control effort: u = -K * x_hat
    float control_signal_u = 0.0;
    for(int i = 0; i < 4; i++) {
        control_signal_u -= K[i] * x_hat[i];
    }
    
    // ... (Keep the gains and integral logic from the previous step) ...

    // 5. Apply LQI Integral correction: u = u - K_int * x_int
    control_signal_u -= (K_int * x_int);
    
    // 6. Saturate the output to your 9V hardware limit
    if (control_signal_u > MAX_VOLTAGE)  control_signal_u = MAX_VOLTAGE;
    if (control_signal_u < -MAX_VOLTAGE) control_signal_u = -MAX_VOLTAGE;
    
    // 7. CONVERT VOLTAGE TO PWM (0 to 255)
    int pwm_output = (int)(control_signal_u * (255.0 / MAX_VOLTAGE));
    
    // 8. Send to the motor using the compiler-suggested function
    setMotorPWM(pwm_output);
}