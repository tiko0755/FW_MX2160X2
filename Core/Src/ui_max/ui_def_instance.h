
// page feeder, component name defines
#define PG_MAIN             ("page0")       // main page
#define MAIN_THICKNESS      ("b4")   // product thickness
#define MAIN_R_REV          ("b5")   // R axis angle per rev
#define MAIN_Y_REV          ("b6")   // Y axis movememt per rev
// POSITON 0
#define MAIN_PS0            ("b_pos0")
#define MAIN_PS0_RST        ("b_rst0")
#define MAIN_PS0_ACT        ("b_act0")
// POSITON 1
#define MAIN_PS1            ("b_pos1")
#define MAIN_PS1_ACT        ("b_act1")
// POSITON 2
#define MAIN_PS2            ("b_pos2")
#define MAIN_PS2_ACT        ("b_act2")
// POSITON 3
#define MAIN_PS3            ("b_pos3")
#define MAIN_PS3_ACT        ("b_act3")
// POSITON 4 
#define MAIN_PS4            ("b_pos4")
#define MAIN_PS4_ACT        ("b_act4")

#define MAIN_HOMING         ("b_homing")
#define MAIN_AUTO           ("b_auto")  
#define MAIN_EMERGENCY      ("b_emg")  
#define MAIN_CLAMP          ("b_clamp")  
#define MAIN_PG_SWITCH      ("b_pages")

#define MAIN_REPO0          ("repo0")
#define MAIN_REPO1          ("repo1")
#define MAIN_REPO2          ("repo2")
#define MAIN_REPO3          ("repo3")
#define MAIN_REPO4          ("repo4")






















#define PAGE_USETTING    ("usetting")
#define PAGE_MOTION        ("motion")
#define PAGE_MSETTING    ("fsetting")
#define PG_EDITPSW    ("pswSetting")

#define FEEDR_picEstop        ("p2")
#define FEEDR_picManAuto    ("p6")
#define FEEDR_picStart        ("p5")
#define FEEDR_picBack        ("p4")
#define FEEDR_picSend        ("p1")
#define FEEDR_picStrip        ("p0")
#define FEEDER_tbStatus            ("tStatus")
#define FEEDER_tbSLBuchang        ("t4")
#define FEEDER_tbSLLimit        ("t5")
#define FEEDER_tbSLShudu        ("t6")
#define FEEDER_tbBLDistance        ("t7")
#define FEEDER_tbLock            ("tLock")

// page usetting, component name defines
#define USETTING_picMotionCfg    ("p5")
#define USETTING_picModMan        ("p0")
#define USETTING_picModAuto        ("p2")
#define USETTING_picModFixedLen    ("p3")
#define USETTING_tbBLShudu        ("t1")

// page motion, component name defines
#define MOTION_picRotaeL        ("p2")
#define MOTION_picRotaeR        ("p3")
#define MOTION_picFsetting        ("p1")
#define MOTION_picAxisSel        ("p5")
#define MOTION_picExit            ("p0")
#define MOTION_tbMinSpd            ("t0")
#define MOTION_tbMaxSpd            ("t7")
#define MOTION_tbCurSpd            ("t5")
#define MOTION_tbTargetPos        ("t1")
#define MOTION_tbModSpd            ("t11")
#define MOTION_tbModPos            ("t12")
#define MOTION_tbStatusStall    ("t8")
#define MOTION_tbStatusOverCurrent    ("t2")
#define MOTION_tbStatusRefL        ("t3")
#define MOTION_tbStatusRefR        ("t4")

// page fsetting, component name defines
#define MSETTING_picSave    ("p0")
#define MSETTING_picDefault    ("b0")
#define MSETTING_picExit    ("p1")
#define MSETTING_tbAccMax    ("t6")
#define MSETTING_tbCurrent    ("t7")
#define MSETTING_tbMicroStep    ("t12")
#define MSETTING_tbPulse    ("t8")
#define MSETTING_tbRamp        ("t18")
#define MSETTING_tbPMUL        ("t9")
#define MSETTING_tbPDIV        ("t4")
#define MSETTING_tbMM_PER_TURN    ("t15")
#define MSETTING_tbNTurn    ("t13")

// page feeder picture defines
#define PGMAIN_PIC_MOD_AUTO        1
#define PGMAIN_PIC_MOD_MAN        2
#define PGMAIN_PIC_START0    42
#define PGMAIN_PIC_START1    43
#define PGMAIN_PIC_ESTOP_RELEASE    31
#define PGMAIN_PIC_ESTOP_PRESS    32
#define PGMAIN_PIC_BACK0    40
#define PGMAIN_PIC_BACK1    38
#define PGMAIN_PIC_STRIP0    36
#define PGMAIN_PIC_STRIP1    6
#define PGMAIN_PIC_STRIP2    7
#define PGMAIN_PIC_SEND0    37
#define PGMAIN_PIC_SEND1    34
#define PGMAIN_PIC_SEND2    35
#define PGMAIN_PIC_UNLOCK    3
#define PGMAIN_PIC_LOCK        4
#define PGMAIN_PIC_NEXT        20
#define PGMAIN_PIC_MOD_LEN    53

// usetting picture defines
#define PGUS_PIC_MAN_UP        48
#define PGUS_PIC_MAN_DWN    49
#define PGUS_PIC_AUTO_UP    44
#define PGUS_PIC_AUTO_DWN    45
#define PGUS_PIC_FLEN_UP    46
#define PGUS_PIC_FLEN_DWN    47

// page motion picture defines
#define PGMOTION_PIC_AXIS0    19
#define PGMOTION_PIC_AXIS1    22
#define PGMOTION_PIC_AXIS2    21

// page motion picture defines
#define PGMSETTING_PIC_SAVED    16
#define PGMSETTING_PIC_SAVE    14

#define EDITPSW_tb_OLD    ("t1")
#define EDITPSW_tb_NEW1    ("t3")
#define EDITPSW_tb_NEW2    ("t6")
#define EDITPSW_tb_INFO    ("t7")
#define EDITPSW_btn_CFM    ("b0")

