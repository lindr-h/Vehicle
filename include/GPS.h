
#define TSIP_VERNUM "7.68a"


#define GPS_PI 	(3.1415926535898)
#define	D2R		(GPS_PI/180.0)
#define	R2D		(180.0/GPS_PI)
#define WEEK 	(604800.)
#define MAXCHAN  (8)

/* control characters for TSIP packets */
#define DLE 	(0x10)
#define ETX 	(0x03)

#define MAX_RPTBUF (256)
#define OUTBUFSIZE  (1000)

extern char outbuf[OUTBUFSIZE];

/* values of TSIPPKT.status */
#define TSIP_PARSED_EMPTY 	0
#define TSIP_PARSED_FULL 	1
#define TSIP_PARSED_DLE_1 	2
#define TSIP_PARSED_DATA 	3
#define TSIP_PARSED_DLE_2 	4

typedef struct
{
    unsigned char latdir;	// 纬度
    char lat[15];
    unsigned char londir;	// 经度
    char lon[15];
    unsigned char altdir;	// 高度,海拔
    char alt[15];
    
 }POSITION;
 
 
 typedef struct
{
    unsigned char eastdir;
    char evel[15];
    unsigned char northdir;
    char nvel[15];
    unsigned char updir;
    char uvel[15];
    
 }VELOCITY;
 
   
/* TSIP packets have the following structure, whether report or command. */
typedef struct {
	short
		counter,				/* counter */
      len;				/* size of buf; < MAX_RPTBUF unsigned chars */
	unsigned char
		status,			 /* TSIP packet format/parse status */
		code;				/* TSIP code */
	unsigned char
		buf[MAX_RPTBUF];	/* report or command string */
} TSIPPKT;

/* TSIP binary data structures */
typedef struct {
	unsigned char
		t_oa_raw, SV_health;
	float
		e, t_oa, i_0, OMEGADOT, sqrt_A,
		OMEGA_0, omega, M_0, a_f0, a_f1,
		Axis, n, OMEGA_n, ODOT_n, t_zc;
	short
		weeknum, wn_oa;
} ALM_INFO;

typedef struct {     /*  Almanac health page (25) parameters  */
	unsigned char
		WN_a, SV_health[32], t_oa;
} ALH_PARMS;

typedef struct {     /*  Universal Coordinated Time (UTC) parms */
	double
		A_0;
	float
		A_1;
	short
		delta_t_LS;
	float
		t_ot;
	short
		WN_t, WN_LSF, DN, delta_t_LSF;
} UTC_INFO;

typedef struct {      /*  Ionospheric info (float)  */
	float
		alpha_0, alpha_1, alpha_2, alpha_3,
		beta_0, beta_1, beta_2, beta_3;
} ION_INFO;

typedef struct {      /*  Subframe 1 info (float)  */
	short
		weeknum;
	unsigned char
		codeL2, L2Pdata, SVacc_raw, SV_health;
	short
		IODC;
	float
		T_GD, t_oc, a_f2, a_f1, a_f0, SVacc;
} EPHEM_CLOCK;

typedef	struct {     /*  Ephemeris info (float)  */
	unsigned char
		IODE, fit_interval;
	float
		C_rs, delta_n;
	double
		M_0;
	float
		C_uc;
	double
		e;
	float
		C_us;
	double
		sqrt_A;
	float
		t_oe, C_ic;
	double
		OMEGA_0;
	float
		C_is;
	double
		i_0;
	float
		C_rc;
	double
		omega;
	float
		OMEGADOT, IDOT;
	double
		Axis, n, r1me2, OMEGA_n, ODOT_n;
} EPHEM_ORBIT;

typedef struct {     /* Navigation data structure */
	short
		sv_number;     /* SV number (0 = no entry) */
	float
		t_ephem;       /* time of ephemeris collection */
	EPHEM_CLOCK
		ephclk;        /* subframe 1 data */
	EPHEM_ORBIT
		ephorb;        /* ephemeris data */
} NAV_INFO;

typedef struct {
	float
		flt1;
	unsigned char
		chr1, chr2, chr3, chr4;
	float
		flt2, flt3, flt4, flt5;
	unsigned char
		chr5;
} TSIP_POS_FILT_PARMS;

typedef struct {
	unsigned char
		bSubcode,
		operating_mode,
		dgps_mode,
		dyn_code,
		trackmode;
	float
		elev_mask,
		cno_mask,
		dop_mask,
		dop_switch;
	unsigned char
		dgps_age_limit;
} TSIP_RCVR_CFG;

struct cph1_datablock_struct {
	unsigned char prn, flagbyte, reserved;
	signed char elev;
	short azim;
	unsigned char signal_strength;
	double pseudorange, carrier_phase;
	float Doppler;
};

struct cph1_msg_struct {
	unsigned char subcode, preamble;
	unsigned short length;
	double receive_time, clock_offset;
	unsigned char numsvs;
	struct cph1_datablock_struct db[MAXCHAN];
	unsigned short checksum;
	unsigned char postamble;
};

/*******************        PROTOYPES            *****************/
/*******************        PROTOYPES            *****************/
/*******************        PROTOYPES            *****************/

/*
 * This routine sends a command to the receiver.  It calls a
 * function sendb() that must be supplied by the user.  All
 * calls to this subroutine are within TSIP_IFC.C.
 */

void send_cmd
	(TSIPPKT *cmd, short sendb(unsigned char bb));

/* functions to add a byte from the receiver and "unstuffs" it */
void tsip_input_proc (TSIPPKT *rpt, char newbyte);

/**/
/**/
/* prototypes for command-encode primitives with suffix convention:  */
/* c = clear, s = set, q = query, e = enable, d = disable            */
void cmd_0x1Dc (TSIPPKT *cmd);
void cmd_0x1Ds (TSIPPKT *cmd,
	float offset);
void cmd_0x1E  (TSIPPKT *cmd,
	unsigned char reset_type);
void cmd_0x1F  (TSIPPKT *cmd);
void cmd_0x20  (TSIPPKT *cmd, unsigned char sv_prn);
void cmd_0x21  (TSIPPKT *cmd);
void cmd_0x22  (TSIPPKT *cmd,
	unsigned char nav_code);
void cmd_0x23  (TSIPPKT *cmd,
	float pos_ECEF[3]);
void cmd_0x24  (TSIPPKT *cmd);
void cmd_0x25  (TSIPPKT *cmd);
void cmd_0x26  (TSIPPKT *cmd);
void cmd_0x27  (TSIPPKT *cmd);
void cmd_0x28  (TSIPPKT *cmd);
void cmd_0x29  (TSIPPKT *cmd);
void cmd_0x2As  (TSIPPKT *cmd,
	float alt);
void cmd_0x2Ad  (TSIPPKT *cmd);
void cmd_0x2Aq  (TSIPPKT *cmd);
void cmd_0x2B  (TSIPPKT *cmd,
	float lat,
	float lon,
	float alt);
void cmd_0x2Cq (TSIPPKT *cmd);
void cmd_0x2Cs (TSIPPKT *cmd,
	unsigned char dyn_code,
	float elev_mask,
	float snr,
	float dop_mask,
	float dop_switch);
void cmd_0x2D  (TSIPPKT *cmd);
void cmd_0x2E  (TSIPPKT *cmd,
	float time_of_week,
	short week_num);
void cmd_0x2F  (TSIPPKT *cmd);
void cmd_0x31  (TSIPPKT *cmd,
	float ECEF_pos[3]);
void cmd_0x32  (TSIPPKT *cmd, 
	float lat,
	float lon,
	float alt);
void cmd_0x35q (TSIPPKT *cmd);
void cmd_0x35s (TSIPPKT *cmd, 
	unsigned char pos_code,
	unsigned char vel_code,
	unsigned char time_code,
	unsigned char opts_code);
void cmd_0x37  (TSIPPKT *cmd);
void cmd_0x38q (TSIPPKT *cmd, 
	unsigned char data_type,
	unsigned char sv_prn);
void cmd_0x38s (TSIPPKT *cmd, 
	unsigned char data_type,
	unsigned char sv_prn,
	unsigned char data_length,
	unsigned char *databuf);
void cmd_0x39  (TSIPPKT *cmd, 
	unsigned char op_code,
	unsigned char sv_prn);
void cmd_0x3A  (TSIPPKT *cmd, 
	unsigned char sv_prn);
void cmd_0x3B  (TSIPPKT *cmd, 
	unsigned char sv_prn);
void cmd_0x3C  (TSIPPKT *cmd, 
	unsigned char sv_prn);
void cmd_0x3Dq (TSIPPKT *cmd);
void cmd_0x3Ds (TSIPPKT *cmd, 
	unsigned char baud_out,
	unsigned char baud_inp,
	unsigned char char_code,
	unsigned char stopbitcode,
	unsigned char output_mode,
	unsigned char input_mode);
void cmd_0x3E  (TSIPPKT *cmd);
void cmd_0x62q (TSIPPKT *cmd);
void cmd_0x62s (TSIPPKT *cmd,
	unsigned char dgps_mode);
void cmd_0x65  (TSIPPKT *cmd,
	unsigned char sv_prn);
void cmd_0x6Eq (TSIPPKT *cmd,
	unsigned char subcode);
void cmd_0x6Ed (TSIPPKT *cmd,
	unsigned char subcode);
void cmd_0x6Es (TSIPPKT *cmd,
	unsigned char subcode,
	unsigned char interval);
void cmd_0x70q  (TSIPPKT *cmd);
void cmd_0x70s  (TSIPPKT *cmd,
	unsigned char dyn_switch,
	unsigned char static_switch,
	unsigned char alt_switch,
	unsigned char extra) ;
void cmd_0x71q (TSIPPKT *cmd);
void cmd_0x71s (TSIPPKT *cmd,
	short pf,
	TSIP_POS_FILT_PARMS *params);
void cmd_0x73q (TSIPPKT *cmd);
void cmd_0x73c (TSIPPKT *cmd);
void cmd_0x73d (TSIPPKT *cmd);
void cmd_0x73s (TSIPPKT *cmd,
	float alt_filter_parm);
void cmd_0x77q (TSIPPKT *cmd);
void cmd_0x77s (TSIPPKT *cmd,
	short dgps_max_age);
void cmd_0x7A_0s (TSIPPKT *cmd,
	unsigned char nmea_interval,
	unsigned long nmea_mask);
void cmd_0x7A_0q (TSIPPKT *cmd) ;
void cmd_0x8E03q (TSIPPKT *cmd);
void cmd_0x8E03s (TSIPPKT *cmd,
	unsigned char baud_out,
	unsigned char baud_inp,
	unsigned char char_code,
	unsigned char stopbitcode,
	unsigned char output_mode,
	unsigned char input_mode);
void cmd_0x8E15q (TSIPPKT *cmd);
void cmd_0x8E15s (TSIPPKT *cmd, unsigned char datum_index);
void cmd_0x8E15s_2 (TSIPPKT *cmd, double datum_parms[5]);
void cmd_0x8E19q (TSIPPKT *cmd);
void cmd_0x8E19d (TSIPPKT *cmd);
void cmd_0x8E19e (TSIPPKT *cmd);
void cmd_0x8E20q (TSIPPKT *cmd);
void cmd_0x8E20d (TSIPPKT *cmd);
void cmd_0x8E20e (TSIPPKT *cmd);
void cmd_0x8E26s (TSIPPKT *cmd);
void cmd_0x8E40q (TSIPPKT *cmd);
void cmd_0x8E40s (TSIPPKT *cmd,
	unsigned char Flags,
	unsigned char HeartBeatSentence,
   short TopOfHourOffset,
	short Frequency,
   char VehicleID[4]);
void cmd_0x8E  (TSIPPKT *cmd,
	unsigned char hexbyte[],
	unsigned char hexnum);
void cmd_0xBBq (TSIPPKT *cmd,
	unsigned char subcode) ;
void cmd_0xBBs (TSIPPKT *cmd,
	TSIP_RCVR_CFG *TsipxBB) ;
void cmd_0xBCs (TSIPPKT *cmd, 
	unsigned char port_num,
	unsigned char in_baud,
	unsigned char out_baud,
	unsigned char data_bits,
	unsigned char parity,
	unsigned char stop_bits,
	unsigned char flow_control,
	unsigned char bReserved1,
	unsigned char bReserved2,
	unsigned char reserved);
void cmd_0xBCq (TSIPPKT *cmd, unsigned char port_num);
void cmd_0xxx (TSIPPKT *cmd,
	unsigned char hexcode, unsigned char hexbyte[], unsigned char hexnum);

/**/
/*  prototypes for report-decode primitives */
/* in TSIP_IFC.C */
short rpt_0x3D (TSIPPKT *rpt,
	unsigned char *tx_baud_index,
	unsigned char *rx_baud_index,
	unsigned char *char_format_index,
	unsigned char *stop_bits,
	unsigned char *tx_mode_index,
	unsigned char *rx_mode_index);
short rpt_0x40 (TSIPPKT *rpt,
	unsigned char *sv_prn,
	short *week_num,
	float *t_zc,
	float *eccentricity,
	float *t_oa,
	float *i_0,
	float *OMEGA_dot,
	float *sqrt_A,
	float *OMEGA_0,
	float *omega,
	float *M_0);
short rpt_0x41 (TSIPPKT *rpt,
	float *time_of_week,
	float *UTC_offset,
	short *week_num);
short rpt_0x42 (TSIPPKT *rpt,
	float ECEF_pos[3],
	float *time_of_fix);
short rpt_0x43 (TSIPPKT *rpt,
	float ECEF_vel[3],
	float *freq_offset,
	float *time_of_fix);
short rpt_0x45 (TSIPPKT *rpt,
	unsigned char *major_nav_version,
	unsigned char *minor_nav_version,
	unsigned char *nav_day,
	unsigned char *nav_month,
	unsigned char *nav_year,
	unsigned char *major_dsp_version,
	unsigned char *minor_dsp_version,
	unsigned char *dsp_day,
	unsigned char *dsp_month,
	unsigned char *dsp_year);
short rpt_0x46 (TSIPPKT *rpt,
	unsigned char *status_code,
	unsigned char *error_code);
short rpt_0x47 (TSIPPKT *rpt,
	unsigned char *nsvs,
	unsigned char *sv_prn,
	float *snr);
short rpt_0x48 (TSIPPKT *rpt,
	unsigned char *message);
short rpt_0x49 (TSIPPKT *rpt,
	unsigned char *sv_health);
short rpt_0x4A(TSIPPKT *rpt,
    float *lat,
    float *lon,
    float *alt,
    float *clock_bias,
    float *time_of_fix);
short rpt_0x4A_2(TSIPPKT *rpt,
    float *alt,
    float *dummy,
    unsigned char *alt_flag);
short rpt_0x4B (TSIPPKT *rpt,
	unsigned char *machine_id,
	unsigned char *status_1,
	unsigned char *status_2);
short rpt_0x4C (TSIPPKT *rpt,
	unsigned char *dyn_code,
	float *el_mask,
	float *snr_mask,
	float *dop_mask,
	float *dop_switch);
short rpt_0x4D (TSIPPKT *rpt,
	float *osc_offset);
short rpt_0x4E (TSIPPKT *rpt,
	unsigned char *response);
short rpt_0x4F (TSIPPKT *rpt,
	double *a0,
	float *a1,
	float *time_of_data,
	short *dt_ls,
	short *wn_t,
	short *wn_lsf,
	short *dn,
	short *dt_lsf);
short rpt_0x53 (TSIPPKT *rpt,
	float *a2d);
short rpt_0x54 (TSIPPKT *rpt,
	float *clock_bias,
	float *freq_offset,
	float *time_of_fix);
short rpt_0x55 (TSIPPKT *rpt,
	unsigned char *pos_code,
	unsigned char *vel_code,
	unsigned char *time_code,
	unsigned char *aux_code);
short rpt_0x56 (TSIPPKT *rpt,
	float vel_ENU[3],
	float *freq_offset,
	float *time_of_fix);
short rpt_0x57 (TSIPPKT *rpt,
	unsigned char *source_code,
	unsigned char *diag_code,
	short *week_num,
	float *time_of_fix);
short rpt_0x58 (TSIPPKT *rpt,
	unsigned char *op_code,
	unsigned char *data_type,
	unsigned char *sv_prn,
	unsigned char *data_length,
	unsigned char *data_packet);
short rpt_0x59 (TSIPPKT *rpt,
	unsigned char *code_type,
	unsigned char status_code[32]);
short rpt_0x5A (TSIPPKT *rpt,
	unsigned char *sv_prn,
	float *sample_length,
	float *signal_level,
	float *code_phase,
	float *Doppler,
	double *time_of_fix);
short rpt_0x5B (TSIPPKT *rpt,
	unsigned char *sv_prn,
	unsigned char *sv_health,
	unsigned char *sv_iode,
	unsigned char *fit_interval_flag,
	float *time_of_collection,
	float *time_of_eph,
	float *sv_accy);
short rpt_0x5C (TSIPPKT *rpt,
	unsigned char *sv_prn,
	unsigned char *slot,
	unsigned char *chan,
	unsigned char *acq_flag,
	unsigned char *eph_flag,
	float *signal_level,
	float *time_of_last_msmt,
	float *elev,
	float *azim,
	unsigned char *old_msmt_flag,
	unsigned char *integer_msec_flag,
	unsigned char *bad_data_flag,
	unsigned char *data_collect_flag);
short rpt_0x5E (TSIPPKT *rpt,
	unsigned char *num_reused_msmts,
	unsigned char *diff_Doppler_unavailable,
	unsigned char *fix_still_converging,
	unsigned char *num_old_msmts);
short rpt_0x6D (TSIPPKT *rpt,
	unsigned char *manual_mode,
	unsigned char *nsvs,
	unsigned char *ndim,
	unsigned char sv_prn[],
	float *pdop,
	float *hdop,
	float *vdop,
	float *tdop);
short rpt_0x6E (TSIPPKT *rpt,
	unsigned char *subcode,
	unsigned char *enable,
	unsigned char *interval);
short rpt_0x6F01 (TSIPPKT *rpt,
	struct cph1_msg_struct *cph);
short rpt_0x70	(TSIPPKT *rpt,
	unsigned char *dyn_switch,
	unsigned char *static_switch,
	unsigned char *alt_switch,
	unsigned char *extra) ;
short rpt_0x72 (TSIPPKT *rpt,
	short *pf,
	TSIP_POS_FILT_PARMS *params);
short rpt_0x74 (TSIPPKT *rpt,
	float *alt_filter_parm);
short rpt_0x78 (TSIPPKT *rpt,
	short *maxage);
short rpt_0x7B (TSIPPKT *rpt,
	unsigned char *subcode,
	unsigned char *interval,
	unsigned long *nmeamask);
	
short rpt_0x82 (TSIPPKT *rpt,
	unsigned char *diff_mode);
short rpt_0x83 (TSIPPKT *rpt,
	double ECEF_pos[3],
	double *clock_bias,
	float *time_of_fix);
short rpt_0x84 (TSIPPKT *rpt,
	double *lat,
	double *lon,
	double *alt,
	double *clock_bias,
	float *time_of_fix);

short rpt_0x85 (TSIPPKT *rpt,
	unsigned char *sv_prn,
	unsigned char *summary_status,
	unsigned char *station_health,
	unsigned char *SV_UDRE_parm,
	unsigned char *iode1,
	unsigned char *iode2,
	float *z_cnt_tow,
	float *range_corr,
	float *range_rate_corr,
	float *delta_range_corr);
short rpt_0x8F03 (TSIPPKT *rpt,
	unsigned char *tx_baud_index,
	unsigned char *rx_baud_index,
	unsigned char *char_format_index,
	unsigned char *stop_bits,
	unsigned char *tx_mode_index,
	unsigned char *rx_mode_index);
short rpt_0x8F14 (TSIPPKT *rpt,
	short *datum_idx,
	double datum_coeffs[5]);
short rpt_0x8F15 (TSIPPKT *rpt,
	short *datum_idx,
	double datum_coeffs[5]);
short rpt_0x8F17 (TSIPPKT *rpt,
	unsigned char *UTM_grid_letter,
	short *UTM_grid_number,
	float *UTM_northing_value,
	float *UTM_easting_value,
	float *UTM_altitude,
	float *clock_bias,
	float *time_of_fix);
short rpt_0x8F18 (TSIPPKT *rpt,
	unsigned char *UTM_grid_letter,
	short *UTM_grid_number,
	double *UTM_northing_value,
	double *UTM_easting_value,
	double *UTM_altitude,
	double *clock_bias,
	float *time_of_fix);
short rpt_0x8F19 (TSIPPKT *rpt,
	short *UTM_enabled);
short rpt_0x8F20 (TSIPPKT *rpt,
	unsigned char *subpacket_id,
	unsigned char *info,
	double *lat,
	double *lon,
	double *alt,
	double vel_enu[],
	double *time_of_fix,
	short *week_num,
	unsigned char *nsvs, unsigned char sv_prn[],
	short sv_IODC[],
	short *datum_index);
short rpt_0x8F26(TSIPPKT *rpt,
	unsigned long *see_write);
short rpt_0x8F40 (TSIPPKT *rpt,
   	unsigned char *Flags,
      unsigned char *HeartBeatSentence,
      short *TopOfHourOffset,
      short *Frequency,
      char *VehicleID);
short rpt_0xBB	(TSIPPKT *rpt,
	TSIP_RCVR_CFG *TsipBB) ;
short rpt_0xBC   (TSIPPKT *rpt,
	unsigned char *port_num,
	unsigned char *in_baud,
	unsigned char *out_baud,
	unsigned char *data_bits,
	unsigned char *parity,
	unsigned char *stop_bits,
	unsigned char *flow_control,
	unsigned char *protocols_in,
	unsigned char *protocols_out,
	unsigned char *reserved);


void rpt_packet(TSIPPKT *rpt,char *obuf);
void tsip_input_proc(TSIPPKT *rpt,char inbyte);



