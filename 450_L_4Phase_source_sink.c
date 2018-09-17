#include "udf.h"
#include "metric.h"

 static real z_min_SS= 3.484;

 /* --------Globals for Large_Bubble_source ------*/
 static int LJ_source_zone_ID=28;

 static real A    = 0.12663281;

 /* --------Globals for Large_Bubble_source ------*/
 static real V_G      = 0.050;           /* 18 mm/s */
 static real rho_G    = 1.225;
 static real U_z_LB;                      /* source for Z-momentum */
 static real Q_G;
 static real m_dot_LB;

 static real z_min_sparger = 2.9130625,z_max_sparger= 2.9511625;
 static real r_sparger= 0.03175;
 static real dZ_mesh = 0.0078;

 static int  LB_source_zone_ID = 28;

 static real min(real x, real y)
 {
 if (x < y) return x;
 else return y;
 }
 static real max(real x, real y)
 {
 if (x > y) return x;
 else return y;
 }

 /* --- End of Globals for Large_Bubble_source ------*/

 DEFINE_EXECUTE_ON_LOADING(Set_udm_names, libname)
 {
  Set_User_Memory_Name(0,"Source_Sink_marker");
  Set_User_Memory_Name(1,"lb_source_marker");
  Set_User_Memory_Name(2,"lb_sink");
  Set_User_Memory_Name(3,"lb_dS");
 }

 DEFINE_ADJUST(adjust_cal_source_sink,d)
 {
  Thread *tm;
  cell_t c;
  Thread *t_lb;
  int lb_phase_index=1;
  real lb_sink,lb_dS;

  #if !RP_HOST
  thread_loop_c(tm,d)
  {
   begin_c_loop_int(c,tm)                          /* Loop over all cells */
   {
    t_lb = THREAD_SUB_THREAD(tm,lb_phase_index);

    lb_sink  = - C_UDMI(c,tm,0)*C_R(c,t_lb)*C_VOF(c,t_lb)/CURRENT_TIMESTEP ;
    lb_dS    = - C_UDMI(c,tm,0)*C_R(c,t_lb)/CURRENT_TIMESTEP;

    C_UDMI(c,tm,2) =lb_sink;
    C_UDMI(c,tm,3) =lb_dS;

   }end_c_loop_int(c,t)
  }
  # endif /* !RP_HOST */
 }


 DEFINE_ON_DEMAND(calc_Source_Sink_Vol)
 {

  Domain *domain;
  domain = Get_Domain(1);   /* domain is declared as a variable  */
  Message0("zone ID is: %d \n",LJ_source_zone_ID);
  Thread *t;
  t = Lookup_Thread(domain,LJ_source_zone_ID);
  cell_t c;
  real x[ND_ND];

  int  n_cells_in_vol=0;
  real Vol=0;


  #if !RP_HOST
   /*---------Reset UDM  --------*/
   begin_c_loop_int(c,t)
   {
    C_UDMI(c,t,0)=0;
   }end_c_loop_int(c,t)
  	/*------END of Reset UDM  ----*/

   begin_c_loop_int(c,t)                          /* Loop over all cells */
   {
    C_CENTROID(x,c,t);

    if(x[2] > z_min_SS)
    {
     C_UDMI(c,t,0)  +=1;
     Vol            +=C_VOLUME(c,t);
     n_cells_in_vol +=1;
    }
   }end_c_loop_int(c,t)
  # endif /* !RP_HOST */


  # if RP_NODE /* NODE SYNCHRONIZATOIN -- Does nothing in Serial */
   Message0("zone ID is: %d \n",LJ_source_zone_ID);
   Vol              = PRF_GRSUM1(Vol);
   n_cells_in_vol   = PRF_GISUM1(n_cells_in_vol);
  # endif /* RP_NODE */

  node_to_host_real_1(Vol);           /* transfer synchronised node data to host */


  # if !RP_NODE
  Message("z_min_SS         = %g\n",z_min_SS); /* gets printed first, yup weird */
  Message("Total Vol        = %g\n",Vol);
  Message("n_cells_in_vol   = %d\n",n_cells_in_vol);
  # endif /*!RP_NODE */
 }

 DEFINE_SOURCE(lb_sink, c, thread, dS, eqn)
 {
  real source;
  Thread *tm = THREAD_SUPER_THREAD(thread);

  source  = C_UDMI(c,tm,2);
  dS[eqn] = C_UDMI(c,tm,3);

  return source;
 }

 DEFINE_SOURCE(sb_source,c, thread, dS, eqn)
 {
  real source;
  Thread *tm = THREAD_SUPER_THREAD(thread);
  /* Thread *t_lb =THREAD_SUB_THREAD(tm,2);  /* phase_domain_index for sb = 2 , lb =1, cont =0 */

  source  = - 0.5*C_UDMI(c,tm,2) ;

  /* dS[eqn] = + C_UDMI(cell,tm,0)*C_R(cell,t_lb)/CURRENT_TIMESTEP; */

  return source;
 }

 DEFINE_SOURCE(mb_source,c, thread, dS, eqn) /* medium size bubble source */
 {
  real source;
  Thread *tm = THREAD_SUPER_THREAD(thread);
  /* Thread *t_lb =THREAD_SUB_THREAD(tm,2);  /* phase_domain_index for sb = 2 , lb =1, cont =0 */

  source  = - 0.5*C_UDMI(c,tm,2) ;

  /* dS[eqn] = + C_UDMI(cell,tm,0)*C_R(cell,t_lb)/CURRENT_TIMESTEP; */

  return source;
 }

 /* -x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
--------------------------LARGE BUBBLE SOURCE ---------------------------------------------------------
x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x */
/* -------  Part -2 Globals for Large Bubbles ------------------------*/
static int  n_cells_patched_LB=0;
static real patch_Volume_LB=0;
/* -------  End of part-2 Globals for Large Bubbles ------------------*/

DEFINE_ON_DEMAND(calc_sparger_patch_volume_LB)
{

	 patch_Volume_LB =0;          /* Reset Global Variables */
  n_cells_patched_LB =0;     /* Reset Global Variables */

	 Domain *domain;
	 domain = Get_Domain(1);   /* domain is declared as a variable  */
  Message0("LB_zone ID is: %d \n",LB_source_zone_ID);
  Thread *t;
 	t = Lookup_Thread(domain,LB_source_zone_ID );
  cell_t c;
  real x[ND_ND];

  real r_loc;

  #if !RP_HOST
				/*---------Reset UDM  --------*/
				begin_c_loop_int(c,t)
				{
						C_UDMI(c,t,1)=0;
				}
				end_c_loop_int(c,t)
				/*------END of Reset UDM  ----*/

				begin_c_loop_int(c,t)      /* Loop over all cells */
				{
						C_CENTROID(x,c,t);
						r_loc = pow(pow(x[0],2)+pow(x[1],2),0.5);

						if ((x[2]>z_min_sparger)&&(x[2]<z_max_sparger)&&(r_loc<r_sparger))
							{
									C_UDMI(c,t,1)+= 1;

								 if(C_UDMI(c,t,1)==1)
								    {
								      patch_Volume_LB    +=C_VOLUME(c,t);
								      n_cells_patched_LB +=1;
								    }
							}
				}end_c_loop_int(c,t)
  #endif /* !RP_HOST */
  #if RP_NODE            /* NODE synchronization */
		n_cells_patched_LB = PRF_GISUM1(n_cells_patched_LB);
		patch_Volume_LB    = PRF_GRSUM1(patch_Volume_LB);
  #endif /* RP_NODE */

		node_to_host_int_1(n_cells_patched_LB);
		node_to_host_real_1(patch_Volume_LB);

		Q_G      = A*V_G;
		m_dot_LB = rho_G*Q_G;
  U_z_LB   = Q_G/(patch_Volume_LB/dZ_mesh);   /*  U_z = Q_G/A_act  */

  #if !RP_NODE  /* These values are avail in both HOST and NODES */
  Message("----Message from Host----------\n");
		Message("n Cells patched for LB      = %d\n",n_cells_patched_LB);
		Message("Total Patch Volume for LB   = %g\n",patch_Volume_LB);
		Message("Total Patch Area   for LB   = %g\n",patch_Volume_LB/dZ_mesh);
  Message("U_z_LB                      = %g\n",U_z_LB);
  Message("Volumetric flow rate Q_G    = %g\n",Q_G);
  Message("Mass flow rate m_dot_LB     = %g\n",m_dot_LB);
  Message("-------------------------------\n");
  #endif /* !RP_NODE */

}
/*-----------End of UDM macros-----------------------------------------*/


DEFINE_SOURCE(LB_Zmom_source,c,t,dS,eqn)
{
 #if !RP_HOST
      real source;
						source  = C_UDMI(c,t,1)*m_dot_LB*U_z_LB/patch_Volume_LB; /* C_VOLUME(c,t); */
						dS[eqn] = 0;
						return source;
 #endif
}


DEFINE_SOURCE(LB_mass_source,c,t,dS,eqn)
{
 #if !RP_HOST
      real source;
						source  = C_UDMI(c,t,1)*m_dot_LB/patch_Volume_LB;
						dS[eqn] = 0;
						return source;
 #endif
}
