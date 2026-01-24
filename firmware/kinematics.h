#ifndef __KINEMATICS__
#define __KINEMATICS__

/**
 * @name forward_kin
 * @brief Calcualte FK for 5 bar linkage. 
 * @param[in] s1 the angle of the first shoulder.
 * @param[in] s2 the angle of the second shoulder.
 * @param[out] x the x coord of the EF.
 * @param[out] y the y coord of the EF.
 */
void forward_kin(double s1, double s2, double &x, double &y);

/**
 * @name inverse_kin
 * @brief Calcualte IK for 5 bar linkage. 
 * @param[in] x the x coord of the EF.
 * @param[in] y the y coord of the EF.
 * @param[out] s1 the angle of the first shoulder.
 * @param[out] s2 the angle of the second shoulder.
 * @return success code (out of bounds). 
 */
int inverse_kin(double x, double y, double &s1, double &s2);

#endif