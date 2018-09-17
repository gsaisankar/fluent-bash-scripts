#!/bin/bash

rm flu_1p1_azi

iso_surface_list=" "

 for ((i=1;i<=15;i++))
 do
 
  r_val=$(echo "scale=6;0.08282/16*$i" | bc -l)
  
  r2_val=$(echo "scale=6;(0.08282/16*$i)^2" | bc -l)
  
  r_name=$(echo "scale=0;$r_val*10^6/1" | bc -l)
  
  # r_name_list=$r_name_list" "r_${r_name}
  
  iso_surface_list=$iso_surface_list" "y_1p1_r_${r_name}
  
  echo "/surface quadric r_${r_name}_cyl 1 0 1 0 0 0 0 0 0 -0${r2_val} 0 " >> flu_1p1_azi
  
  echo "/surface iso-surface mixture y-coordinate y_1p1_r_${r_name} r_${r_name}_cyl () solid () 1.1 () " >> flu_1p1_azi
  
  
  
 done
 
  # echo $r_name_list >> f_pp_text
  # echo $iso_surface_list >> flu_1p1_azi
  
  echo "/report/surface-integrals area-weighted-avg  $iso_surface_list () phase-1 y-velocity  yes phase_1_Vy_1p1 " >> flu_1p1_azi

  
 
