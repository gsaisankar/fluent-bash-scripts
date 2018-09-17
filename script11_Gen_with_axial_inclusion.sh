#!/bin/bash
 #y axis is the height of the cylinder - CSA on XY plane.
 
ARRAY=(-0.4747 -0.0747 0.0253 0.3253 0.5253)
ARRAY2=(1 5 6 9 11) 
 for ((j=0;j<5;j++)) do 
 rm flu_p${ARRAY2[j]}_azi
echo "file/read-case poly_0.5_3ring_200k-4.cas"  >> flu_p${ARRAY2[j]}_azi

echo "file/read-data poly_0.5_3ring_200k-4-35.000000.dat"  >> flu_p${ARRAY2[j]}_azi
iso_surface_list=" "

 for ((i=1;i<=15;i++))
 do
 
  r_val=$(echo "scale=6;0.08282/16*$i" | bc -l)
  
  r2_val=$(echo "scale=6;(0.08282/16*$i)^2" | bc -l)
  
  r_name=$(echo "scale=0;$r_val*10^6/1" | bc -l)
  
  # r_name_list=$r_name_list" "r_${r_name}
  
  iso_surface_list=$iso_surface_list" "y_p5_r_${r_name}

#everything that follows /surface and /report is a command for FLUENT surface creation / post processing.
  
  echo "/surface quadric r_${r_name}_cyl 1 0 1 0 0 0 0 0 0 -0${r2_val} 0 " >> flu_p${ARRAY2[j]}_azi
  
  echo "/surface iso-surface mixture y-coordinate y_p5_r_${r_name} r_${r_name}_cyl () solid () ${ARRAY[j]} () " >> flu_p${ARRAY2[j]}_azi
  
  
 done
 
  # echo $r_name_list >> f_pp_text
  # echo $iso_surface_list >> flu_p${ARRAY2[j]}_azi

  
  echo "/report/surface-integrals area-weighted-avg  $iso_surface_list () phase-1 y-velocity  yes phase_1_Vy " >> flu_p${ARRAY2[j]}_azi

 done 
 

#declare -A matrix
#num_rows=1
#num_columns=5
#matrix[$num_rows,$num_columns]=0.1 0.5 0.6 0.9 1.1
