
#define DEBUG 				0	/* 0 means do not debug */
int verbosity;                   /* verbosity level. 0 low, 10 is high */

#include "log.h"

/* In python_43 the assignment of the WindPtr size has been moved from a fixed
value determined by values in python.h to a values which are adjustable from
within python */
int ndim;			// Define the fundamental dimension of the grid
int mdim;
int NDIM, MDIM, NDIM2;
int NPLASMA;                    //The number of cells with non-zero volume or the size of plasma structure

char basename[132];		// The root of the parameter file name being used by python

/* These are tunable parameters that control various aspects of python
 * and the assorted programs.  In some cases they affect the "care" with
 * which a calculation is made, and this can in principle affect the
 * speed of the program.  One day if may be desirable to change some
 * of these parameters with time in the program.  At present they are
 * simply collected here
 * 
 * */

double dfudge;			// This is the push-through distance
double DFUDGE;
#define VCHECK	1.e6		// The maximum allowable error in calculation of the velocity in calculate_ds

/* 57h -- Changed several defined variables to numbers to allow one to vary them 
in the process of running the code */
double SMAX_FRAC; 		/* In translate_in_wind, a limit is placed on the maximum distance a
				photon can travel in one step.  It is a fraction SMAX_FRAC of the
				distance of the photon from the origin.  This had been hardwired to
				0.1 for up to 57h.  Changing it to 0.5 speeds up the current version
				of the code by as much as a factor of 2 for small sized grids.  This
				had been introduced as part of the attempt to assure ourselves that
				line shapes were calculated as accurately as possilble. 
				*/ 
double DENSITY_PHOT_MIN; 		/* This constant is a minimum density for the purpose of calculating
 				photoionization heating and recombination cooling.  It is important that heating and cooling
				be calculated self-consistently.  Program speed is somewhat sensitive 
				to this parameter, at the 10% level if raised from 1e-3 to 1.  There is a 
				trade-off since lower minima may give better results, especially for macro atoms. */

//#define SMAX_FRAC	0.1  
#define LDEN_MIN        1e-3   /* The minimum density required for a line to be conidered for scattering
				  or emission in calculate_ds and lum_lines */


/* End of "care factor" definition */


#define RMIN   				1.e9
#define RMAX   				3.e10
#define VWIND  				2.e8
#define MDOT  				1.e-9*MSOL/YR
#define TSTAR 				30000.	/*Sets a floor on the frequency range used to calculate
						   ionization balance in the wind.  Only used in python.c.  May 
						   be superfluous */
#define BETA  				1.0
#define KAPPA_CONT 			4.
#define EPSILON  			1.e-6  /* A general purpose fairly small number */
#define NSTAT 				9
#define VMAX                		1.e9
#define TAU_MAX				20.   /* Sets an upper limit in extract on when
			a photon can be assumed to be completely absorbed */

//#define SELECT_NEBULAR                1 /*non zero means to use the nebular approximation; 0 implies
//                                                              use LTE populations based on t_rad*/
#define DANG_LIVE_OR_DIE   2.0	/* If constructing photons from a live or die run of the code, the
				   angle over which photons will be accepted must be defined */


//#define NPHOT                                 10000
int NPHOT;			/* As of python_40, NPHOT must be defined in the main program using
				   python.h */

#define NWAVE  			       10000 //Increasing from 4000 to 10000 (SS June 04)
#define MAXSCAT 			50

/* Define the structures */


/* Geometry is an actual structure which exists at the beginning of th program
   It carries the variables which define the geometry.  Reasonable values of each of
   these should be defined before it is altered with inputs fromt eh terminal. 
   The geometry structure is used to transfer all of the information about a wind


 */

/* Definitions of the coordinate system types */

#define SPHERICAL		0
#define CYLIND			1
#define	RTHETA			2
#define	CYLVAR                  3

/* Definitions of spectral types, which are all negative because when
 * one reads a spectrum from a list of models these are numbered beginning
 * with zero, see the discussion in get_models.c   080518 - ksl - 60a
 */

#define SPECTYPE_BB      -1
#define SPECTYPE_UNIFORM -2
#define SPECTYPE_NONE	 -3

/* Number of model_lists that one can have, should be the same as NCOMPS in models.h */
#define NCOMPS 	10
#define LINELENGTH 	160

struct geometry
{
/* 67 - ksl This section added to allow for restarting the program, and adds parameters used
 * in the calculation */

	int wcycle, pcycle;      /* The number of completed ionization and spectrum cycles */
	int wcycles, pcycles;	/* The number of ionization and spectrum cycles desired */

/* Begin description of the actual geometery */

  int coord_type, ndim,mdim;      /* The type of geometry and dimensionality of the wind array. 
				0=1-d spherical, 1=cylindrical, 2 = spherical polar, 3=cylindrical
				but the z coordiante changes with rho in an attempt to allow for
				a vertically extended disk....
				ndim is the dimensionality of the first dimension.  In the CV case
				it is in the plane of the disk. Mdim is generally along the z axis
				*/
  int nplasma, nmacro;          /*The number of cells in the plasma and macro structures 08mar ksl*/
  double rmax, rmax_sq;		/* The maximum distance to which a photon should be followed */
  double mstar, rstar, rstar_sq, tstar, gstar;	/* Basic parameters for the WD */
  double twind;			/* temperature of wind */
  int binary_system;            /*0--> single star system
				  1--> binary system
				  */
  int disk_type;		/*0 --> no disk, 
				  1 --> a standard disk in xy plane, 
				  2 --> a vertically extended disk 
				  Note that this definition is new to Python52 */
  int disk_illum;        	/*Treatment of effects of illumination on the disk. 
				  0--> Disk simply absorbs the radiation and it is lost
				  1--> Disk reradiates the radiation immediately via electron scattering
				  2--> Disk radiation is absorbed and changes the temperature of the disk for
				      future ionization cycles
				  3--> Disk illumination is treated in terms of an analytic approximation
				  04Aug ksl -- this parameter added for Python52
				*/
  int disk_tprofile;
  double disk_mdot;		/* mdot of  DISK */
  double diskrad, diskrad_sq;
  double disk_z0,disk_z1;	/* For vertically extended disk, z=disk_z0*(r/diskrad)**disk_z1 */
  int wind_type;		/*Basic prescription for wind(0=SV,1=speherical , 2 can imply old file */
  int log_linear;		/*0 -> the grid spacing will be logarithmic in x and z, 1-> linear */
  double xlog_scale, zlog_scale;	/* Scale factors for setting up a logarithmic grid, the [1,1] cell
					   will be located at xlog_scale,zlog_scale */
  int star_radiation, disk_radiation;	/* 1 means consider radiation from star, disk,  bl, and/or wind */
  int bl_radiation, wind_radiation;
  int matom_radiation;          /* Added by SS Jun 2004: for use in macro atom computations of detailed spectra
				   - 1 means use emissivities for BOTH macro atom levels and kpkts. 0 means don't
				   (which is correct for the ionization cycles. */
  int ioniz_mode;		/* describes the type of ionization calculation which will
				   be carried out.  0=on the spot, 1=LTE, 2=fixed ionization
				   fractions,  3 means to recalculate the ionization structure 
				   based on the energy absorbed in the wind (mod_on_the_spot), 4
				   is a test.  It is currently set to do the same as 3, except
				   that ground state mulitpliciites are used instead of 
				    a partition function */
  int macro_ioniz_mode;         /* Added by SS Apr04 to control the use of macro atom populations and
				   ionization fractions. If it is set to 1 then macro atom populations
				   computed from estimators are used. If set to 0 then the macro atom
				   populations are computed as for minor ions. By default it is set to
				   0 initially in python.c and then set to 1 the first time that
				   Monte Carlo estimators are normalised. */
  int ioniz_or_extract;           /* Set to 1 (true) during ionization cycles, set to 0 (false) during calculation of
				  detailed spectrum.  Originally introduced by SS in July04 as he added
				  macro atoms.  Name changed by ksl (57h) since this variable can be used more
				  generally to speed up the extract portion of the calculation.
				*/
  int macro_simple;            /* Added by SS May04 for diagnostics. As default this is set to 0. A full
				  Macro Atom calculation is performed in that case. If it is set to 1 it means
				  that although Macro Atom data has been read in, all lines/continua are treated
				  using the simplified two-level approximation. Such a calculation should reproduce
				  the same results as pre-Macro Atom versions of the code. */
  int partition_mode;            /* Diagnostic to force the partition function to be calculated in
				a specific way. */
  int line_mode;		/*0, the atomosphere is a completely absorbing and no photons
				   will be scattered.  In this mode, assuming the wind is a source
				   of emission, the emissivity will be the Einstein A coefficient
				   1, the atmosphere is completely scattering, there will be no
				   interchange of energy between the photons and the electrons
				   as a result of readiation transfer
				   2, then a simple single scattering approximation is applied in which
				   case the scattered flux is just  A21/(C21+A21). 
				   3, then radiation trapping is included as well.
				   6, If set to 6 initially, this switches on the macro atom stuff
				   and then behaves like 3. (SS)
				 */
  int scatter_mode;		/*The way in which scattering for resonance lines is treated 
				   0  isotropic
				   1  anisotropic
				   2  thermally broadened anisotropic
				 */
  int rt_mode;			/* radiative transfer mode (0=Sobolev,1=simple (used only by balance) */
                                /* This IS now used by Python - set to 2 for Macro Atom method. Set to 1
				   for non-Macro Atom methods (SS) */
  /* The spectral types are SPECTYPE_BB for bb, SPECTYPE_UNIFORM for a uniform spectral distribution, 0 or more from a filelist.
   * A value of SPECTYPE_NONE indicates no emission is expected from this particular source */
  int star_ion_spectype, star_spectype;	/* The type of spectrum used to create the continuum
					   for the star in the ionization and final spectrum calculation */
  int disk_ion_spectype, disk_spectype;	/* The type of spectrum used to create the continuum
					   for the disk in the ionization and final spectrum calculation */
  int bl_ion_spectype, bl_spectype;	/* The type of spectrum used to create the continuum
					   for the bl in the ionization and final spectrum calculation */
  char model_list[NCOMPS][LINELENGTH];	/* The file which contains the model names and the associated values for the model */

  /* Generic parameters for the wind */
  double wind_mdot, stellar_wind_mdot;		/* Mass loss rate in disk and stellar wind */
  double wind_rmin, wind_rmax;	/*Spherical extent of the wind */
  double wind_rho_min, wind_rho_max;	/*Min/Max rho for wind in disk plane */
  double wind_thetamin, wind_thetamax;	/*Angles defining inner and outer cones of wind, measured from disk plane */
  double mdot_norm;		/*A normalization factor used in SV wind, and Knigge wind */
  int adiabatic;                 /*0-> Do not include adiabatic heating in calculating the cooling of the wind
				  1-> Use adiabatic heating in calculating the cooling of the wind
				  */
  int auger_ionization;          /*0 -> Do not include innershell photoionization /Auger effects; 1-> include them*/
  /* Parameters defining Shlossman & Vitello Wind */
  double sv_lambda;		/* power law exponent describing from  what portion of disk wind is radiated */
  double sv_rmin, sv_rmax, sv_thetamin, sv_thetamax, sv_gamma;	/* parameters defining the goemetry of the wind */
  double sv_v_zero;		/* velocity at base of wind */
  double sv_r_scale, sv_alpha;	/* the scale length and power law exponent for the velocity law */
  double sv_v_infinity;		/* the factor by which the velocity at infinity exceeds the excape velocity */

  /* Paramater for the Elvis AGN wind - closely based on SV */
  double elvis_offset;   /*This is a vertical offset for a region where the
			   wind rises vertically from the disk*/
 
  /* Parameters defining Knigge Wind */
  double kn_dratio;		/* parameter describing collimation of wind */
  double kn_lambda;		/* power law exponent describing from  what portion of disk wind is radiated */
//    double kn_rmin, kn_rmax, kn_thetamin, kn_thetamax, kn_gamma;      /* parameters defining the goemetry of the wind */
//    double kn_v_zero;         /* velocity at base of wind */
  double kn_r_scale, kn_alpha;	/* the scale length and power law exponent for the velocity law */
  double kn_v_infinity;		/* the factor by which the velocity at infinity exceeds the excape velocity */

  /* Parameters describing Castor and Larmors spherical wind */
  double cl_v_zero, cl_v_infinity, cl_beta;	/* Power law exponenet */
  double cl_rmin, cl_rmax;

  /*Parameters defining a corona in a ring above a disk */
  double corona_rmin, corona_rmax;	//the minimum and maximu radius of the corona

  double corona_base_density, corona_scale_height;	//the density at the base of the corona and the scale height

  double corona_vel_frac;	// the radial velocity of the corona in units of the keplerian velocity

/* Initial values for defining wind structure for a planar geometry.  These are currently only used by balance and this
   may not be the best approach generally and depending on where this ends up. Some consolidation is desirable */
  double pl_vol, pl_vmax;
  double pl_t_r, pl_t_e, pl_w;
  double pl_nh;

  double lum_tot, lum_star, lum_disk, lum_bl, lum_wind;	/* The total luminosities of the disk, star, bl, & wind 
							   are actually not used in a fundamental way in the program */
  double lum_ff, lum_fb, lum_lines;	/* The luminosity of the wind as a result of ff, fb, and line radiation */
  double f_tot, f_star, f_disk, f_bl, f_wind;	/* The integrated specific L between a freq min and max which are
						   used to establish the fraction of photons of various types */
  
  double f_matom, f_kpkt; /*Added by SS Jun 2004 - to be used in computations of detailed spectra - the
				energy emitted in the band via k-packets and macro atoms respectively. */

// The next set of parameters relate to the secondary
  double m_sec, q;		/* Mass of the secondary, mass ratio of system */
  double period;		/* Period of the systems in seconds */
  double a, l1, l2, phi;	/* Separation of primary and secondary, distance of l1 from primary,phi at l1 */
  double l1_from_m2, r2_far;	/* Distance to l1 from m2, distance to far side of secondary from primary */
  double r2_width;		/* Maximum width of Roche surface of secondary in the plane of orbit */

  double t_bl;			/*temperature of the boundary layer */
  double weight;		/*weight factor for photons/defined in define_phot */

// The next set of parameters describe the input datafiles that are read
  char atomic_filename[132];   /* 54e -- The masterfile for the atomic data*/
  char fixed_con_file[132];    /* 54e -- For fixed concentrations, the file specifying concentrations */
}
geo;


struct plane
{
  double x[3];			/* A position included in the plane (usally the "center" */
  double lmn[3];		/* A unit vector perpindicular to the plane (usuall in the "positive" direction */
}
plane_l1, plane_sec, plane_m2_far;	/* these all define planes which are perpendicular to the line of sight from the 
					   primary to the seconday */


/* Note that since we are interested in biconical flows, our definition of a cone is not exactly
 * what one might guess.  The cone is defined in the positive z direction but reflected through 
 * the xy plane.  
 * 56d -- Beginning with 56d, ksl has switched to a new definition of cones, that is intended to
 * make it possible to use ds_to_cone easier as part of coordiaate systems.  The new definition
 * is based on the intersection of the cone with the z axis rather than the intersection with
 * the disk plane.  At present both definitions are used in the program and therefore both shold
 * be defined.  Once the new definition is promulgated through the entire program, and verified
 * the old definitions can be elimiated.  05jul -- ksl
 */

typedef struct cone
{
  double z;                     /* The place where the cone intersects the z axis (used after 56d)*/
  double dzdr;                  /* the slope (used after 56d) */
}
cone_dummy, *ConePtr;

ConePtr cones_rtheta;		/*A ptr to the cones that define the theta directions in rtheta coods*/

struct cone windcone[2];



#define NRINGS	301		/* The actual number of rings completely defined
				   is NRINGS-1 ... or from 0 to NRINGS-2.  This is
				   because you need an outer radius...but the rest
				   of this element is not filled in. */

struct xdisk
{
  double r[NRINGS];		/* The inner radius of an annulus */
  double t[NRINGS];		/* The temperature at the middle of the annulus */
  double g[NRINGS];		/* The gravity at the middle of the annulus */
  double v[NRINGS];		/* The velocity at the middle of the annulus */
  double heat[NRINGS];		/* The total energy flux of photons hitting each annulus */
  double ave_freq[NRINGS];	/* The flux weighted average of frequency of photons hiiting each annulus */
  double w[NRINGS];		/* The radiative weight of the photons that hit the disk */
  double t_hit[NRINGS];		/* The effective T of photons hitting the disk*/
  int nphot[NRINGS];		/*The number of photons created in each annulus */
  int nhit[NRINGS];		/*The number of photons which hit each annulus */
}
disk, qdisk;			/* disk defines zones in the disk which in a specified frequency band emit equal amounts
				   of radiation. qdisk stores the amount of heating of the disk as a result of
				  illumination by the star or wind */

#define NBLMODEL 100

struct blmodel
{
   int n_blpts;
   float r[NBLMODEL];
   float t[NBLMODEL];
}
blmod;




/* 	This structure defines the wind.  The structure w is allocated in the main
	routine.  The total size of the structure will be NDIM x MDIM, and the two
	dimenssions do not need to be the same.  The order of the
	cells in the structure is such that the as you increse the cell number by one
	z increases the fastest.

57+ -- The wind structure was reduced to contain just information about the geometry.  
Variables for wind cells that actually have volume in the wind are now in plasmamain, 
or macromain.  The wind structure still contains a volume element, which is the volume
of that cell in the wind, This is used in some cases to determine whether the cell
has any portion in the wind.  

Note that most of the macro atom information is in a separate structure.  This was
done to make it easier to control the size of the entire structure   06jul-ksl

 */
#define NIONIZ	5		/*The number of ions (normally H and He) for which one separately tracks ionization 
				   and recombinations */
#define LPDF   3		/*The number of bins into which the line luminosity is divided in the course pdf
				   created by total_line_emission 
				Reduced from 10 to 3 by SS for testing data with few lines.*/
                            

/* 061104 -- 58b -- ksl -- Added definitions to characterize whether a cell is in the wind. */
#define W_ALL_INWIND   0	//Entire grid cell is in the wind
#define W_PART_INWIND  1	//Part of gridcell is in the wind
#define W_NOT_INWIND  -1        //None of gridcell is in the wind
#define W_IGNORE      -2	//Even though the wind may occupy a small part of this cell, assume
				//photons simply pass through the cell.  This is new in 58b

typedef struct wind
{
  int nwind;                   /*A self-reference to this cell in the wind structure*/
  int nplasma;                 /*A cross refrence to the corresponding cell in the plasma structure */
  double x[3];			/*position of inner vertex of cell */
  double xcen[3];		/*position of the "center" of a cell (Added by ksl for 52a--04Aug)*/
  double r, rcen;		/*radial location of cell (Used for spherical, spherical polar
				  coordinates. (Added by ksl for 52a --04Aug)*/
  double theta,thetacen;	/*Angle of coordinate from z axis (Added by ksl for 52a -- 04Aug)*/
  struct cone wcone;                   /*56d -- cone structure that defines the bottom edge of the cell in 
				  CYLVAR coordinates*/
  double v[3];			/*velocity at inner vertex of cell.  For 2d coordinate systems this
 				  is defined in the xz plane */
  double v_grad[3][3];		/*velocity gradient tensor  at the inner vertex of the cell NEW */
  double div_v;			/*Divergence of v at center of cell */
  double dvds_ave;		/* Average value of dvds */
  double dvds_max, lmn[3];	/*The maximum value of dvds, and the direction in a cell in cylindrical coords */
  double vol;			/* valid volume of this cell (more specifically the volume of one of the
				   two annular regions that this cell represents). */
  int inwind;			/* 061104 -- 58b -- ksl -- Moved definitioins of for whether a cell is or is not
				inwind to #define statements above */

}
wind_dummy, *WindPtr;

WindPtr wmain;

/* 57+ - 06jun -- plasma is a new structure that contains information about the properties of the
plasma in regions of the geometry that are actually included n the wind 

	07jul	ksl	Added volume to the structure.  The value of this should be the same
			as the corresponding volume in the Wind structure, as we are still
			using the Wind volume for some tests of whether the a photon can
			interact in a cell.
*/

typedef struct plasma {
  int nwind;                   /*A cross reference to the corresponding cell in the  wind structure*/
  int nplasma;                 /*A self reference to this  in the plasma structure */
  double ne;			/* electron density in the shell */
  double rho;			/*density at the center of the cell */
  double vol;			/* valid volume of this cell (more specifically the volume of one of the
				   two annular regions that this cell represents). */
  double density[NIONS];	/*The number density of a specific ion.  This needs to correspond
				   to the ion order obtained by get_atomic_data */
  double partition[NIONS];	/*The partition function for each  ion */
  double levden[NLTE_LEVELS];	/*The number density (occupation number?) of a specific level */


  double recomb_simple[NTOP_PHOT]; /* "alpha_e - alpha" (in Leon's notation) for b-f processes in simple atoms. */

/* Begining of macro information */
  double kpkt_emiss;              /*This is the specific emissivity due to the conversion k-packet -> r-packet in the cell
				    in the frequency range that is required for the final spectral synthesis. (SS) */

  double kpkt_abs;                   /* k-packet equivalent of matom_abs. (SS) */

  int kbf_use[NTOP_PHOT];            /* List of the indices of the photoionization processes to be used for kappa_bf. (SS) */
   int kbf_nuse;                      /* Total number of photoionization processes to be used for kappa_bf. (SS) */

/* End of macro information */


  double t_r, t_r_old;		/*radiation temperature of cell */
  double t_e, t_e_old;		/*electron temperature of cell */
  double dt_e, dt_e_old;	/*How much t_e changed in the previous iteration */
  double heat_tot, heat_tot_old;	/* heating from all sources */
  double heat_lines, heat_ff;
  double heat_lines_macro, heat_photo_macro; /* bb and bf heating due to macro atoms. Subset of heat_lines 
						and heat_photo. SS June 04. */
  double heat_photo, heat_z;	/*photoionization heating total and of metals */
  double w;			/*The dilution factor of the wind */
  int ntot;			/*Total number of photon passages */
  int nrad;			/* Total number of photons radiated within the cell */
  int nioniz;			/* Total number of photons capable of ionizing H */
  double ioniz[NIONS], recomb[NIONS];	/* Number of ionizations and recombinations for each ion.
					   The sense is ionization from ion[n], and recombinations 
					   to each ion[n] */
  int scatters[NIONS];			/* 68b - The number of scatters in this cell for each ion.*/
  double xscatters[NIONS];	/* 68b - Diagnostic measure of energy scattered out of beam on extract */
  double heat_ion[NIONS];       /* The amount of energy being transferred to the electron pool
				   by this ion via photoionization*/
  double lum_ion[NIONS];       /* The amount of energy being released from the electron pool
				   by this ion via recombination*/
  double j, ave_freq, lum;	/*Respectively mean intensity, 
				intensity_averaged frequency, 
				   luminosity and       absorbed luminosity of shell */
  double lum_lines, lum_ff, lum_adiabatic;
  double lum_fb, lum_z;		/*fb luminosity & fb of metals metals */
  double lum_rad, lum_rad_old;	/* The specfic radiative luminosity in frequencies defined by freqmin
				   and freqmax.  This will depend on the last call to total_emission */
  double dmo_dt[3];		/*Radiative force of wind */
  int npdf;			/* The number of points actually used in the luminosity pdf */
  int pdf_x[LPDF];		/* The line numbers of *line_ptr which form the boundaries the luminosity pdf */
  double pdf_y[LPDF];		/* Where the pdf is stored -- values between 0 and 1 */
  double gain;			/* The gain being used in interations of the structure */
  double converge_t_r, converge_t_e, converge_hc;	/* Three measures of whether the program believes the grid is converged */
  int converge_whole, converging;	/* converge_whole=0 if subroutine convergence feels point is converged, converging is an
				   indicator of whether the program thought the cell is on the way to convergence 0 implies converging */
  double gamma_inshl[NAUGER]; /*MC estimator that will record the inner shell ionization rate - very similar to macro atom-style estimators */
  //int kpkt_rates_known;
  //COOLSTR kpkt_rates;
} plasma_dummy, *PlasmaPtr;

PlasmaPtr plasmamain;

/* A storage area for photons.  The idea is that it is sometimes time-consuming to create the
cumulative distribution function for a process, but trivial to create more than one photon 
of a particular type once one has the cdf,  This appears to be case for f fb photons.  But 
the same procedure could be used for line photons */

#define NSTORE 10
typedef struct photon_store {
  int n;  			/* This is the photon number that was last used */
  double t, f1, f2, freq[NSTORE];

} photon_store_dummy, *PhotStorePtr;

PhotStorePtr photstoremain; 

/* 
060616 -- ksl -- 57g -- Modified the plamsa structure to add a new macro stucture that is only needed for macro
atoms. This structure contains most of the variables that were previously dominating the total size of the plasma
array. This effectively solves a problem with producing a huge wind_save file, as well as making the size of the
executable much smaller for the simple atom case.

0608 -- ksl -- 57h -- There is now and extended  discussion in gridwind.c about possible ways to restructure 
the macro structure in order to dynamically set the size of arrays like jbar and jbar_old.  For now, recompilation 
of the code is required.  

0803 -- ksl -- 60 -- The first index is the level, or config  number.  get_atomicdata assures that macro levels, 
if they exist have lower level numbers than other types of levels.  

*/


typedef struct macro {
  double *jbar;
  //[NLEVELS_MACRO][NBBJUMPS]; 
  /* This will store the Sobolev mean intensity in transitions which is needed 
				       for Macro Atom jumping probabilities. The indexing is by configuration (the 
				       NLTE_LEVELS) and then by the upward bound-bound jumps from that level 
				       (the NBBJUMPS) (SS)*/
  double *jbar_old;
  //[NLEVELS_MACRO][NBBJUMPS];

  double *gamma;
  //[NLEVELS_MACRO][NBFJUMPS]; 
  /* This is similar to the jbar but for bound-free transitions. It records the 
					 appropriate photoionisation rate co-efficient. (SS)*/
  double *gamma_old;
  //[NLEVELS_MACRO][NBFJUMPS];
  double *gamma_e;
  //[NLEVELS_MACRO][NBFJUMPS]; 
  /* This is Leon's gamma_e: very similar to gamma but energy weighted. Needed
					   for division of photoionisation energy into excitation and k-packets. (SS)*/
  double *gamma_e_old;
  //[NLEVELS_MACRO][NBFJUMPS];

  double *alpha_st;
  //[NLEVELS_MACRO][NBFJUMPS]; /* Same as gamma but for stimulated recombination rather than photoionisation. (SS)*/
  double *alpha_st_old;
  //[NLEVELS_MACRO][NBFJUMPS];
  double *alpha_st_e;
  //[NLEVELS_MACRO][NBFJUMPS]; 
  /* Same as gamma_e but for stimulated recombination rather than photoionisation. (SS)*/
  double *alpha_st_e_old;
  //[NLEVELS_MACRO][NBFJUMPS];

  double *recomb_sp;
  //[NLEVELS_MACRO][NBFJUMPS]; 
  /* Spontaneous recombination. (SS) */
  double *recomb_sp_e;
  //[NLEVELS_MACRO][NBFJUMPS]; 
  /* "e" version of the spontaneous recombination coefficient. (SS) */

  double *matom_emiss;
  //[NLEVELS_MACRO]; 
  /* This is the specific emissivity due to the de-activation of macro atoms in the cell
					in the frequency range that is required for the final spectral synthesis. (SS) */
  double *matom_abs;
  //[NLEVELS_MACRO];  
  /* This is the energy absorbed by the macro atom levels - recorded during the ionization 
					cycles and used to get matom_emiss (SS) */
  int kpkt_rates_known;
  COOLSTR kpkt_rates;
} macro_dummy, *MacroPtr;

MacroPtr macromain;

int xxxpdfwind;			// When 1, line luminosity calculates pdf

int size_Jbar_est,  size_gamma_est, size_alpha_est;

// These definitions define a photon type, generally it's origin
#define PTYPE_STAR	    0
#define PTYPE_BL	    1
#define PTYPE_DISK          2
#define PTYPE_WIND	    3

/* These definitions define the current or final state of a photon.  They are used by
phot.istat below */

#define P_INWIND            0	//in wind,
#define P_SCAT              1	//in process of scattering,
#define P_ESCAPE            2	//Escaped to reach the universe,
#define P_HIT_STAR          3	//absorbed by photosphere of star,
#define P_HIT_DISK          7	//Banged into disk
#define P_ABSORB            6	//Photoabsorbed within wind
#define P_TOO_MANY_SCATTERS 4	//in wind after MAXSCAT scatters
#define P_ERROR             5	//Too many calls to translate without something happening
#define P_SEC               8	//Photon hit secondary

#define TMAX_FACTOR			1.5	/*Factor by which t_e can exceed
						   t_r in order for absorbed to 
						   match emitted flux */
#define TMIN				2000.
/* ??? TMIN appears to be used both for the minimum temperature and for 
   calculating the fraction of recombinations that go to the ground state.  This
   looks like a problem ksl-98jul???? */


#define NDIM_MAX 200
double wind_x[NDIM_MAX], wind_z[NDIM_MAX]; /* These define the edges of the cells in the x and z directions */
double wind_midx[NDIM_MAX], wind_midz[NDIM_MAX];	/* These define the midpoints of the cells in the x and z directions */

/* Next two lines are for cyl_var coordinates.  They are used in locating the appropriate 
 * locating the appropriate cell, for example by cylvar_where_in_grid
 */

double wind_z_var[NDIM_MAX][NDIM_MAX];     
double wind_midz_var[NDIM_MAX][NDIM_MAX];

typedef struct photon
{
  double x[3];		/* Vector containing position of packet */
  double lmn[3];	/*direction cosines of this packet */
  double freq;
  double w;		/*weight of this packet */
  double tau;
  int istat;		/*status of photon.  See definitions P_INWIND, etc above */
  int nscat;		/*number of scatterings */
  int nres;		/*The line number in lin_ptr of last scatter or wind line creation */
  int nnscat;           /* Used for the thermal trapping model of
			   anisotropic scattering to carry the number of
			   scattering to "extract" when needed for wind
			   generated photons SS05. */
  int nrscat;		/* number of resonance scatterings */
  int grid;		/*grid position of the photon in the wind, if
			   the photon is in the wind.  If the photon is not
			   in the wind, then -1 implies inside the wind cone and  
			   -2 implies outside the wind */
  int origin;		/* Where this photon originated.  If the photon has
			scattered it's "origin" may be changed to "wind".  The
			definitions should be according to PTYPE ... above. 
			 */
}
p_dummy, *PhotPtr;



/* 68b - ksl - This is a structure in which the history of a single photon bundle can be recorded
 * See phot_util   phot_hist().  It needs to be used carefully.  if phot_hist_on is true
 * then photon histories will be attempted.
 */

#define MAX_PHOT_HIST	1000
int n_phot_hist, phot_hist_on,phot_history_spectrum;
struct photon  xphot_hist[MAX_PHOT_HIST];

struct basis
{
  double a[3][3];

}
basis_cartesian;


/* The next section defines the spectrum arrays.  The spectrum structure contains
   the selection criteria for each spectrum as well as the array in which to store the
   spectrum.  The first MSPEC spectra are reserved for the total emitted spectrum, the
   total spectrum of photons which are scattered and the total spectrum of photons which
   are absorbed.  The remainder of the spectra pertain to the spectrum as seen from various
   directions. Note that the space for the spectra are allocated using a calloc statement
   in spectrum_init*/


/* NSPEC has been removed from python.h because it was not really needed */
//0ld68c #define NSPEC 				14

#define MSPEC                            6
int nspectra;			/* After create_spectrum, the number of elements allocated for s, or 
				   alternatively the number of spectra one has to work with.  Note that
				   general s[0],s[1] and s[2] are the escaping, scattered and absorbed photons,
				   while elements higher than this will contain spectra as seen by different observers */


int nscat[MAXSCAT + 1], nres[MAXSCAT + 1], nstat[NSTAT];

typedef struct spectrum
{
  char name[40];
  float freqmin, freqmax, dfreq;
  double lmn[3];
  double mmax, mmin;		/* Used only in live or die situations, mmax=cos(angle-DANG_LIVE_OR_DIE)
				   and mmim=cos(angle+DANG_LIVE_OR_DIE).   In actually defining this
				   one has to worry about signs and exactly whether you are above
				   or below the plane */
  double renorm;		/* Factor used in Live or die option where it is 4*PI/domega;
				   1.0 otherwise */
  int nphot[NSTAT];		/* Used to help define what happened to photons when trying to construct this
				   particular spectrum */
  int nscat;			/* nscat>999 -> select everything
				   0 < nscat < MAXScat select only those photons which have
				   scattered nscat times, number of scattering photons in spectrum, if nscat is negative
				   then all photons with more than |nscat| are included.  */
  int top_bot;			/* 0 ->select photons regardless of location 
				   >0     -> select only photons whose "last" position is above the disk
				   <0    -> select only photons whose last position is below the disk */
  double x[3],r;               /* The position and radius of a special region from which to extract spectra  */
  double f[NWAVE];
}
spectrum_dummy, *SpecPtr;

SpecPtr s;



/* Note: In python_53a, the ability to project onto a cylindrical coordinate
 * system still does not exist in py_wind.  04nov -- ksl ??
 */

/* Parameters used only by py_wind 
 * py_wind_projecti	0 -> simply print the various parameters without 
 * 			atempting toproject onto a yz plane
 * 			1 -> project onto a yz plane.
 */

int py_wind_min, py_wind_max, py_wind_delta,py_wind_project;
double *aaa;  // A pointer to an array used by py_wind

/* This is the structure needed for a cumulative distribution function. The CDFs are
generated from a function which is usually only proportional to the probability density
function.  It is sometimes useful, e.g. in calculating the reweighting function to
have access to the proper normalization.  Since the one needs the normalization to
properly create the CDF, this was added for python_43.2  */
#define NPDF 200

typedef struct Pdf
{
  double x[NPDF + 1];		/* Positions for which the probability density
				is calculated */
  double y[NPDF + 1];		/* The value of the CDF at x */
  double d[NPDF + 1];		/* 57i -- the rate of change of the probability
				density at x */
  double limit1, limit2; /* Limits (running from 0 to 1) that define a portion
			of the CDF to sample */
  double x1,x2;           /* limits if they exist on what is returned */
  double norm;           //The scaling factor which would renormalize the pdf
}
 *PdfPtr, pdf_dummy;


/* Variable used to allow something to be printed out the first few times
   an even occurs */
int itest, jtest;

char hubeny_list[132];		//Location of listing of files representing hubeny atmospheres




// Allow for a diagnostic file 

FILE *epltptr;     //TEST
int diag_on_off;  // on is non-zero  //TEST

/* These variables are stored or used by the routines for anisotropic scattering */
/* Allow for the transfer of tau info to scattering routine */
double tau_x_dvds;		//tau_x_dvds/dvds is the actual tau
//double tau_scatter_min;		//Set in subroutine scatter for use by extract
struct Pdf pdf_randwind_store[100];
PdfPtr    pdf_randwind;
struct photon phot_randwind;

/* N.B. pdf_randwind and phot_randwind are used in the routine anisowind for 
as part of effort to incorporate anisotropic scattering in to python.  
Added for python_43.2 */


/* Provide generally for having arrays which descibe the 3 xyz axes. 
these are initialized in main  */

double x_axis[3];
double y_axis[3];
double z_axis[3];

/* These are structures associated with frequency limits/bands used for photon
generation and for calculating heating and cooling */

#define NBANDS 10
struct xbands
{
  double f1[NBANDS],f2[NBANDS];
  double min_fraction[NBANDS];
  double nat_fraction[NBANDS];		// The fraction of the accepted luminosity in this band
  double used_fraction[NBANDS];
  double flux[NBANDS];			//The "luminosity" within a band
  double weight[NBANDS];
  int nphot[NBANDS];
  int nbands;		// Actual number of bands in use
}
xband;


/* The next section contains the freebound structures that can be used for both the
 * specific emissivity of a free-bound transition, and for the recombination coefficient
 * assuming the array has been initialized, which can take a few minutes
*/

#define NTEMPS	30 		// The number of temperatures which are stored in each fbstruct
#define NFB	10		// The maximum number of frequency intervals for which the fb emission is calculated

struct fbstruc
{
  double f1, f2;
  double emiss[NIONS][NTEMPS];
}
freebound[NFB];

double xnrecomb[NIONS][NTEMPS];	// There is only one set of recombination coefficients
double fb_t[NTEMPS];
int nfb;			// Actual number of freqency intervals calculated


//This is a new structure to contain the frequency range of the final spectrum
//During the ionization cycles, the emissivity due to k-packets and macro atom
//deactivations in this range will be computed and then used in the final spectral
//synthesis part of the code (SS June04).
struct emiss_range
{
  double fmin, fmax; // min and max frequency required in the final spectrum
}
em_rnge;


#include "version.h"                /*54f -- Added so that version can be read directly */
#include "templates.h"
#include "recipes.h"

// 04apr ksl -- made kap_bf external so can be passed around variables
double kap_bf[NLEVELS];
