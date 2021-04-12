/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <arm_math.h>
#include <core_cm4.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
# include <stdlib.h>


#define TEST_LENGTH_SAMPLES  200
#define BLOCK_SIZE 200 
#define FILTER_TAP_NUM 861
static float32_t firStateF32[BLOCK_SIZE + FILTER_TAP_NUM -1];

static double firCoeffs32[FILTER_TAP_NUM] = {
  0.00823081660517282,
  0.004241153255664437,
  0.0049967657835168705,
  0.005567864835399979,
  0.005875536616866924,
  0.005850486069745185,
  0.005440080226476028,
  0.004614530313882306,
  0.003371716494294435,
  0.0017400445864035816,
  -0.00022105683123942575,
  -0.0024231849744504263,
  -0.004753767563898142,
  -0.007082864079285067,
  -0.00927256147200671,
  -0.01118603689903546,
  -0.012699778394074901,
  -0.013710949850814079,
  -0.014149462499036777,
  -0.013980700638317634,
  -0.013212334317102751,
  -0.011893167163254444,
  -0.010109777720699726,
  -0.007981978842737922,
  -0.005652459117363256,
  -0.003275896026922075,
  -0.0010077785816155298,
  0.0010087598006497094,
  0.0026545109271496584,
  0.0038426085932366577,
  0.004525854626591926,
  0.004700062182754323,
  0.0044035916844086115,
  0.0037123367232186626,
  0.0027335817434238742,
  0.001595085428857242,
  0.0004333784696272693,
  -0.0006189747821241994,
  -0.0014450610485866888,
  -0.001953929718048559,
  -0.0020892343266503154,
  -0.0018334673631400697,
  -0.0012086231585008757,
  -0.00027362269599643865,
  0.000881986874828727,
  0.002147329667583445,
  0.003401263276020902,
  0.0045249237719876285,
  0.005412913293049235,
  0.0059837643254459045,
  0.006187847653395958,
  0.006012410887807678,
  0.005482120297176556,
  0.004656422014693793,
  0.003623450454389641,
  0.0024909157952062873,
  0.0013753744828894614,
  0.0003903051470447329,
  -0.0003653975308359769,
  -0.0008173031727375558,
  -0.0009227302196534508,
  -0.0006750196492298914,
  -0.00010406624183926928,
  0.0007266624952602495,
  0.0017270361939607899,
  0.0027892684362946967,
  0.0037991269139328694,
  0.004647621367459624,
  0.0052421950906097504,
  0.00551618053880463,
  0.00543587407625462,
  0.005004120306193491,
  0.004259885172811446,
  0.0032743591387538497,
  0.002143728341437602,
  0.000979418756574715,
  -0.00010344107943647284,
  -0.0009978937825336827,
  -0.0016163850868994632,
  -0.0018996422393615644,
  -0.0018230183215532754,
  -0.0013990734129757938,
  -0.0006765264034586324,
  0.0002647782030084028,
  0.0013219273014404755,
  0.0023794531710287295,
  0.0033213112750152016,
  0.004042969506907968,
  0.0044622094607614,
  0.004527639894223874,
  0.004224304491014824,
  0.003575442770768358,
  0.0026404348302519537,
  0.0015086889021712808,
  0.00029068147439759856,
  -0.0008934679387341416,
  -0.0019262166081541938,
  -0.0027049552470066835,
  -0.0031527964788830953,
  -0.0032267274764691167,
  -0.002922171185902858,
  -0.002273839041137672,
  -0.0013523541186405345,
  -0.0002570767217403462,
  0.0008939740011782528,
  0.0019762991557113426,
  0.002871833583052578,
  0.0034812915082844042,
  0.0037342523361926284,
  0.003596875090195351,
  0.003075534878625452,
  0.0022166326912485,
  0.0011019386547315142,
  -0.0001595897722525067,
  -0.00144288529234411,
  -0.0026195462436795144,
  -0.0035711384185811803,
  -0.004201546579437247,
  -0.004447397240801794,
  -0.00428469148289335,
  -0.003731412465717955,
  -0.002846288492228971,
  -0.001722817311172108,
  -0.0004797771963002841,
  0.0007512466176266699,
  0.0018390381962432203,
  0.002666523276288476,
  0.003142589066385007,
  0.003211964663986561,
  0.002861200501445673,
  0.002120067623416445,
  0.001059053162401366,
  -0.0002171868556287631,
  -0.0015808034288799812,
  -0.002893845925096016,
  -0.0040225852203543385,
  -0.0048514521376895456,
  -0.0052951377942153915,
  -0.005308177577469094,
  -0.004888697004458524,
  -0.004079354929291533,
  -0.002963049132551897,
  -0.0016548536812194294,
  -0.00029009868494837105,
  0.000988466421272318,
  0.0020457139757133706,
  0.0027688808998729366,
  0.0030790848137448555,
  0.002940203156603594,
  0.0023634054494102792,
  0.001404553239883905,
  0.00015886684509532076,
  -0.0012483410797987059,
  -0.0026752031119850342,
  -0.003974651682510259,
  -0.0050117015010827165,
  -0.005676444057223514,
  -0.005898715999070015,
  -0.005654931752351714,
  -0.004969490269761149,
  -0.003909697648408695,
  -0.002579646659313634,
  -0.001113936961202932,
  0.00033316130208663316,
  0.0016088345203464044,
  0.0025856816121663205,
  0.0031682793134694093,
  0.0032853218916839683,
  0.0029142162402851614,
  0.0021196012327006947,
  0.0009499196573085702,
  -0.00045382000875485563,
  -0.001956967661586531,
  -0.003400859342878748,
  -0.004635683831913569,
  -0.005531500606083494,
  -0.0059926047368367786,
  -0.005967454433130833,
  -0.0054542431734508285,
  -0.004501378336018333,
  -0.003202705192194848,
  -0.0016881754055945066,
  -0.00011032781850597607,
  0.0013705821796699044,
  0.002604619806861318,
  0.0034648680283809277,
  0.0038649817572443375,
  0.0037624249520520534,
  0.0031693590603063065,
  0.002146741957180528,
  0.0008001765886821162,
  -0.0007294679510108396,
  -0.002283085804870226,
  -0.003698080469997203,
  -0.0048243339940851945,
  -0.005541415581873645,
  -0.005770624745724623,
  -0.005482135764453317,
  -0.004699428580036574,
  -0.0034972337282713844,
  -0.001993664582831038,
  -0.0003374471152493748,
  0.0013055775330870734,
  0.002770481708445894,
  0.003909831839175194,
  0.004609619045728825,
  0.0048002214112565,
  0.0044636885912290565,
  0.0036366636186274614,
  0.0024063579227247543,
  0.0009016997219832165,
  -0.0007197813114096608,
  -0.0022880479347695925,
  -0.0036375661565151143,
  -0.004624327561967898,
  -0.005141405793525949,
  -0.0051294640035553845,
  -0.004583125341633804,
  -0.003551562330387976,
  -0.002134074725262968,
  -0.0004697510139774591,
  0.0012763218759592808,
  0.0029301879755476504,
  0.004326960046842565,
  0.005327358832307392,
  0.005832155989010236,
  0.0057925342572675755,
  0.005215355003667223,
  0.00416254638453292,
  0.0027448830998452398,
  0.0011107475380140745,
  -0.0005688341515932767,
  -0.0021178214362861177,
  -0.0033726879157163466,
  -0.0041995212737925254,
  -0.0045078635754674835,
  -0.004260110179899669,
  -0.0034752712428021346,
  -0.00222731258532759,
  -0.0006378111794222377,
  0.0011363707047258882,
  0.0029189301668653012,
  0.004532194974042062,
  0.005815213314397732,
  0.006640262729999733,
  0.006925962745235069,
  0.006645732271894018,
  0.005830361446105214,
  0.004565212237315229,
  0.0029816098531068006,
  0.0012437185891636502,
  -0.00046835791552680663,
  -0.001976803422777828,
  -0.003124160096314482,
  -0.003789436648608591,
  -0.003900601566038502,
  -0.003442027677154025,
  -0.0024561531469110137,
  -0.0010393094776983186,
  0.0006680127478326049,
  0.0024952258737456884,
  0.004259082726514134,
  0.005782276303495958,
  0.006911628506407377,
  0.007533427288713753,
  0.007585155017405015,
  0.007061889544297205,
  0.006016974892887592,
  0.004556705252969327,
  0.0028297896886451106,
  0.00101222065797553,
  -0.0007106285474530595,
  -0.0021627590503681972,
  -0.0031954417551041856,
  -0.0037024047061093436,
  -0.003630736566606629,
  -0.0029861239296683356,
  -0.0018325127957145417,
  -0.0002855464595370629,
  0.001498751372721746,
  0.0033397604841493506,
  0.005050436094146236,
  0.006456403307032396,
  0.0074136107138286535,
  0.007822890194442112,
  0.007640085102186445,
  0.006880854612082917,
  0.005618906046110469,
  0.003978494723149553,
  0.002122127123054331,
  0.0002342178287317056,
  -0.0014972651674417209,
  -0.0028999627404832713,
  -0.0038343863772012043,
  -0.00420832042604433,
  -0.003985892988800661,
  -0.0031917543232510438,
  -0.0019084910909049925,
  -0.0002680309549067255,
  0.0015614323792117351,
  0.0033920520446989533,
  0.00503531567123585,
  0.006320812820510816,
  0.007113459926543471,
  0.007327062422186348,
  0.006933374451183596,
  0.005965398000479574,
  0.004513260727446009,
  0.0027157590884999503,
  0.0007461447773846315,
  -0.0012048658803259062,
  -0.0029482190922131583,
  -0.0043147361115954555,
  -0.005172927759859835,
  -0.005442522823561518,
  -0.005102113144286665,
  -0.004191521793737017,
  -0.0028076529077544453,
  -0.0010943595151727424,
  0.000770359250706992,
  0.002592159979484831,
  0.004180639960762328,
  0.005367865925469182,
  0.006026718509001767,
  0.006080818196134455,
  0.005513360898042543,
  0.00436890997409862,
  0.0027510756838852153,
  0.000810793851662708,
  -0.0012701794708578715,
  -0.0032962568330379676,
  -0.005074440977334228,
  -0.0064350901079020875,
  -0.0072528639555284645,
  -0.007455677681591299,
  -0.007027266833633375,
  -0.006017984877789674,
  -0.00453965209206193,
  -0.0027391771515494867,
  -0.000810908354515525,
  0.0010495633867293045,
  0.0026446256486858857,
  0.0038055502346725236,
  0.004404001697177,
  0.004366776626786619,
  0.003682639543087458,
  0.0024038815827743826,
  0.0006414733194466225,
  -0.001445560120768849,
  -0.003665456869554576,
  -0.005812427409091598,
  -0.007686244637367823,
  -0.00911296323579852,
  -0.009959447346656526,
  -0.010151647036790222,
  -0.009673793291308538,
  -0.008581606416714155,
  -0.006985595548657142,
  -0.005048991322646507,
  -0.002970349908230752,
  -0.0009610363740030039,
  0.0007708706276601449,
  0.00204136959049526,
  0.002711122380762093,
  0.002697492114030201,
  0.0019817788286629633,
  0.0006134592600181664,
  -0.001293701096478316,
  -0.0035729292690895747,
  -0.0060207190473813505,
  -0.008413886622110678,
  -0.0105316339118813,
  -0.012176116531002803,
  -0.013193134675210688,
  -0.013485740468409701,
  -0.013024228140014445,
  -0.011850457159695038,
  -0.010074704647967114,
  -0.007866707105699124,
  -0.005439896716898209,
  -0.003031722737214531,
  -0.0008825440604209002,
  0.0007867819334218507,
  0.0017964231353167265,
  0.0020247487838252268,
  0.0014212690266915708,
  0.00001234110401845865,
  -0.002099580768686369,
  -0.004744019821783855,
  -0.00769574239562267,
  -0.010694482219318202,
  -0.013467950793824134,
  -0.015756242808104785,
  -0.01733602938336906,
  -0.018041653789344064,
  -0.01778176076395989,
  -0.01654976681531032,
  -0.01442749046027815,
  -0.011581146077571769,
  -0.008249561163085188,
  -0.004725583555264271,
  -0.001333053614952361,
  0.0016004319246630905,
  0.0037715496989726397,
  0.004928517764138442,
  0.004894931698852287,
  0.003588797974980247,
  0.0010344852973691316,
  -0.0026333230297707626,
  -0.007174198690151446,
  -0.012255043967884597,
  -0.01747032969507739,
  -0.022368711869528213,
  -0.026484150157319804,
  -0.02936922118484423,
  -0.030628026545232955,
  -0.029946991841096757,
  -0.027120661338819713,
  -0.022071010285378927,
  -0.014858032586836155,
  -0.005681640953123928,
  0.005125861998055412,
  0.017116153199024247,
  0.02974931229386181,
  0.04242475751114372,
  0.0545167041897555,
  0.06541176523017146,
  0.07454621946110591,
  0.08144080597282957,
  0.08573028012480859,
  0.08718606153942189,
  0.08573028012480859,
  0.08144080597282957,
  0.07454621946110591,
  0.06541176523017146,
  0.0545167041897555,
  0.04242475751114372,
  0.02974931229386181,
  0.017116153199024247,
  0.005125861998055412,
  -0.005681640953123928,
  -0.014858032586836155,
  -0.022071010285378927,
  -0.027120661338819713,
  -0.029946991841096757,
  -0.030628026545232955,
  -0.02936922118484423,
  -0.026484150157319804,
  -0.022368711869528213,
  -0.01747032969507739,
  -0.012255043967884597,
  -0.007174198690151446,
  -0.0026333230297707626,
  0.0010344852973691316,
  0.003588797974980247,
  0.004894931698852287,
  0.004928517764138442,
  0.0037715496989726397,
  0.0016004319246630905,
  -0.001333053614952361,
  -0.004725583555264271,
  -0.008249561163085188,
  -0.011581146077571769,
  -0.01442749046027815,
  -0.01654976681531032,
  -0.01778176076395989,
  -0.018041653789344064,
  -0.01733602938336906,
  -0.015756242808104785,
  -0.013467950793824134,
  -0.010694482219318202,
  -0.00769574239562267,
  -0.004744019821783855,
  -0.002099580768686369,
  0.00001234110401845865,
  0.0014212690266915708,
  0.0020247487838252268,
  0.0017964231353167265,
  0.0007867819334218507,
  -0.0008825440604209002,
  -0.003031722737214531,
  -0.005439896716898209,
  -0.007866707105699124,
  -0.010074704647967114,
  -0.011850457159695038,
  -0.013024228140014445,
  -0.013485740468409701,
  -0.013193134675210688,
  -0.012176116531002803,
  -0.0105316339118813,
  -0.008413886622110678,
  -0.0060207190473813505,
  -0.0035729292690895747,
  -0.001293701096478316,
  0.0006134592600181664,
  0.0019817788286629633,
  0.002697492114030201,
  0.002711122380762093,
  0.00204136959049526,
  0.0007708706276601449,
  -0.0009610363740030039,
  -0.002970349908230752,
  -0.005048991322646507,
  -0.006985595548657142,
  -0.008581606416714155,
  -0.009673793291308538,
  -0.010151647036790222,
  -0.009959447346656526,
  -0.00911296323579852,
  -0.007686244637367823,
  -0.005812427409091598,
  -0.003665456869554576,
  -0.001445560120768849,
  0.0006414733194466225,
  0.0024038815827743826,
  0.003682639543087458,
  0.004366776626786619,
  0.004404001697177,
  0.0038055502346725236,
  0.0026446256486858857,
  0.0010495633867293045,
  -0.000810908354515525,
  -0.0027391771515494867,
  -0.00453965209206193,
  -0.006017984877789674,
  -0.007027266833633375,
  -0.007455677681591299,
  -0.0072528639555284645,
  -0.0064350901079020875,
  -0.005074440977334228,
  -0.0032962568330379676,
  -0.0012701794708578715,
  0.000810793851662708,
  0.0027510756838852153,
  0.00436890997409862,
  0.005513360898042543,
  0.006080818196134455,
  0.006026718509001767,
  0.005367865925469182,
  0.004180639960762328,
  0.002592159979484831,
  0.000770359250706992,
  -0.0010943595151727424,
  -0.0028076529077544453,
  -0.004191521793737017,
  -0.005102113144286665,
  -0.005442522823561518,
  -0.005172927759859835,
  -0.0043147361115954555,
  -0.0029482190922131583,
  -0.0012048658803259062,
  0.0007461447773846315,
  0.0027157590884999503,
  0.004513260727446009,
  0.005965398000479574,
  0.006933374451183596,
  0.007327062422186348,
  0.007113459926543471,
  0.006320812820510816,
  0.00503531567123585,
  0.0033920520446989533,
  0.0015614323792117351,
  -0.0002680309549067255,
  -0.0019084910909049925,
  -0.0031917543232510438,
  -0.003985892988800661,
  -0.00420832042604433,
  -0.0038343863772012043,
  -0.0028999627404832713,
  -0.0014972651674417209,
  0.0002342178287317056,
  0.002122127123054331,
  0.003978494723149553,
  0.005618906046110469,
  0.006880854612082917,
  0.007640085102186445,
  0.007822890194442112,
  0.0074136107138286535,
  0.006456403307032396,
  0.005050436094146236,
  0.0033397604841493506,
  0.001498751372721746,
  -0.0002855464595370629,
  -0.0018325127957145417,
  -0.0029861239296683356,
  -0.003630736566606629,
  -0.0037024047061093436,
  -0.0031954417551041856,
  -0.0021627590503681972,
  -0.0007106285474530595,
  0.00101222065797553,
  0.0028297896886451106,
  0.004556705252969327,
  0.006016974892887592,
  0.007061889544297205,
  0.007585155017405015,
  0.007533427288713753,
  0.006911628506407377,
  0.005782276303495958,
  0.004259082726514134,
  0.0024952258737456884,
  0.0006680127478326049,
  -0.0010393094776983186,
  -0.0024561531469110137,
  -0.003442027677154025,
  -0.003900601566038502,
  -0.003789436648608591,
  -0.003124160096314482,
  -0.001976803422777828,
  -0.00046835791552680663,
  0.0012437185891636502,
  0.0029816098531068006,
  0.004565212237315229,
  0.005830361446105214,
  0.006645732271894018,
  0.006925962745235069,
  0.006640262729999733,
  0.005815213314397732,
  0.004532194974042062,
  0.0029189301668653012,
  0.0011363707047258882,
  -0.0006378111794222377,
  -0.00222731258532759,
  -0.0034752712428021346,
  -0.004260110179899669,
  -0.0045078635754674835,
  -0.0041995212737925254,
  -0.0033726879157163466,
  -0.0021178214362861177,
  -0.0005688341515932767,
  0.0011107475380140745,
  0.0027448830998452398,
  0.00416254638453292,
  0.005215355003667223,
  0.0057925342572675755,
  0.005832155989010236,
  0.005327358832307392,
  0.004326960046842565,
  0.0029301879755476504,
  0.0012763218759592808,
  -0.0004697510139774591,
  -0.002134074725262968,
  -0.003551562330387976,
  -0.004583125341633804,
  -0.0051294640035553845,
  -0.005141405793525949,
  -0.004624327561967898,
  -0.0036375661565151143,
  -0.0022880479347695925,
  -0.0007197813114096608,
  0.0009016997219832165,
  0.0024063579227247543,
  0.0036366636186274614,
  0.0044636885912290565,
  0.0048002214112565,
  0.004609619045728825,
  0.003909831839175194,
  0.002770481708445894,
  0.0013055775330870734,
  -0.0003374471152493748,
  -0.001993664582831038,
  -0.0034972337282713844,
  -0.004699428580036574,
  -0.005482135764453317,
  -0.005770624745724623,
  -0.005541415581873645,
  -0.0048243339940851945,
  -0.003698080469997203,
  -0.002283085804870226,
  -0.0007294679510108396,
  0.0008001765886821162,
  0.002146741957180528,
  0.0031693590603063065,
  0.0037624249520520534,
  0.0038649817572443375,
  0.0034648680283809277,
  0.002604619806861318,
  0.0013705821796699044,
  -0.00011032781850597607,
  -0.0016881754055945066,
  -0.003202705192194848,
  -0.004501378336018333,
  -0.0054542431734508285,
  -0.005967454433130833,
  -0.0059926047368367786,
  -0.005531500606083494,
  -0.004635683831913569,
  -0.003400859342878748,
  -0.001956967661586531,
  -0.00045382000875485563,
  0.0009499196573085702,
  0.0021196012327006947,
  0.0029142162402851614,
  0.0032853218916839683,
  0.0031682793134694093,
  0.0025856816121663205,
  0.0016088345203464044,
  0.00033316130208663316,
  -0.001113936961202932,
  -0.002579646659313634,
  -0.003909697648408695,
  -0.004969490269761149,
  -0.005654931752351714,
  -0.005898715999070015,
  -0.005676444057223514,
  -0.0050117015010827165,
  -0.003974651682510259,
  -0.0026752031119850342,
  -0.0012483410797987059,
  0.00015886684509532076,
  0.001404553239883905,
  0.0023634054494102792,
  0.002940203156603594,
  0.0030790848137448555,
  0.0027688808998729366,
  0.0020457139757133706,
  0.000988466421272318,
  -0.00029009868494837105,
  -0.0016548536812194294,
  -0.002963049132551897,
  -0.004079354929291533,
  -0.004888697004458524,
  -0.005308177577469094,
  -0.0052951377942153915,
  -0.0048514521376895456,
  -0.0040225852203543385,
  -0.002893845925096016,
  -0.0015808034288799812,
  -0.0002171868556287631,
  0.001059053162401366,
  0.002120067623416445,
  0.002861200501445673,
  0.003211964663986561,
  0.003142589066385007,
  0.002666523276288476,
  0.0018390381962432203,
  0.0007512466176266699,
  -0.0004797771963002841,
  -0.001722817311172108,
  -0.002846288492228971,
  -0.003731412465717955,
  -0.00428469148289335,
  -0.004447397240801794,
  -0.004201546579437247,
  -0.0035711384185811803,
  -0.0026195462436795144,
  -0.00144288529234411,
  -0.0001595897722525067,
  0.0011019386547315142,
  0.0022166326912485,
  0.003075534878625452,
  0.003596875090195351,
  0.0037342523361926284,
  0.0034812915082844042,
  0.002871833583052578,
  0.0019762991557113426,
  0.0008939740011782528,
  -0.0002570767217403462,
  -0.0013523541186405345,
  -0.002273839041137672,
  -0.002922171185902858,
  -0.0032267274764691167,
  -0.0031527964788830953,
  -0.0027049552470066835,
  -0.0019262166081541938,
  -0.0008934679387341416,
  0.00029068147439759856,
  0.0015086889021712808,
  0.0026404348302519537,
  0.003575442770768358,
  0.004224304491014824,
  0.004527639894223874,
  0.0044622094607614,
  0.004042969506907968,
  0.0033213112750152016,
  0.0023794531710287295,
  0.0013219273014404755,
  0.0002647782030084028,
  -0.0006765264034586324,
  -0.0013990734129757938,
  -0.0018230183215532754,
  -0.0018996422393615644,
  -0.0016163850868994632,
  -0.0009978937825336827,
  -0.00010344107943647284,
  0.000979418756574715,
  0.002143728341437602,
  0.0032743591387538497,
  0.004259885172811446,
  0.005004120306193491,
  0.00543587407625462,
  0.00551618053880463,
  0.0052421950906097504,
  0.004647621367459624,
  0.0037991269139328694,
  0.0027892684362946967,
  0.0017270361939607899,
  0.0007266624952602495,
  -0.00010406624183926928,
  -0.0006750196492298914,
  -0.0009227302196534508,
  -0.0008173031727375558,
  -0.0003653975308359769,
  0.0003903051470447329,
  0.0013753744828894614,
  0.0024909157952062873,
  0.003623450454389641,
  0.004656422014693793,
  0.005482120297176556,
  0.006012410887807678,
  0.006187847653395958,
  0.0059837643254459045,
  0.005412913293049235,
  0.0045249237719876285,
  0.003401263276020902,
  0.002147329667583445,
  0.000881986874828727,
  -0.00027362269599643865,
  -0.0012086231585008757,
  -0.0018334673631400697,
  -0.0020892343266503154,
  -0.001953929718048559,
  -0.0014450610485866888,
  -0.0006189747821241994,
  0.0004333784696272693,
  0.001595085428857242,
  0.0027335817434238742,
  0.0037123367232186626,
  0.0044035916844086115,
  0.004700062182754323,
  0.004525854626591926,
  0.0038426085932366577,
  0.0026545109271496584,
  0.0010087598006497094,
  -0.0010077785816155298,
  -0.003275896026922075,
  -0.005652459117363256,
  -0.007981978842737922,
  -0.010109777720699726,
  -0.011893167163254444,
  -0.013212334317102751,
  -0.013980700638317634,
  -0.014149462499036777,
  -0.013710949850814079,
  -0.012699778394074901,
  -0.01118603689903546,
  -0.00927256147200671,
  -0.007082864079285067,
  -0.004753767563898142,
  -0.0024231849744504263,
  -0.00022105683123942575,
  0.0017400445864035816,
  0.003371716494294435,
  0.004614530313882306,
  0.005440080226476028,
  0.005850486069745185,
  0.005875536616866924,
  0.005567864835399979,
  0.0049967657835168705,
  0.004241153255664437,
  0.00823081660517282
};



uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;
arm_fir_instance_f32 S; 
volatile float R =0;
volatile float SpO2 =0;
volatile float bpm =0;

volatile int NbreMax =0;
volatile int flag =0;
volatile int NbreSec =0;

//volatile float Circulaire[60] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile float Circulaire[60]={0};
volatile int borneSup = 130; // Peuvent etre modifiees par l'utilisateur
volatile int borneInf = 40;


void AlarmeFC(){
    for(;;){
        if((bpm > borneSup)|| (bpm < borneInf)){
            Cy_GPIO_Write(Pin_1_0_PORT, Pin_1_0_NUM,1);
            vTaskDelay(pdMS_TO_TICKS(500));
            Cy_GPIO_Write(Pin_1_0_PORT, Pin_1_0_NUM,0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}


float BPM(float* maximum){
    float BPM =0;
		if(flag ==1){
			flag =0;
			int nombreMax =0;
    for(int i =0; i>30; i++){
        if(maximum[i] != 0){
            nombreMax = nombreMax +1;
        }}
        BPM = nombreMax*6;
        for(int i =0; i>30; i++){ // Reinitialiser le vecteur a des 0.
            maximum[i] = 0;
         }    
		}	
		if(flag ==2){
			flag =0;
			 int nombreMax =0;
    for(int i =30; i>60; i++){
        if(maximum[i] != 0){
            nombreMax = nombreMax +1;
        }}
        BPM = nombreMax*6;
        for(int i =30; i>60; i++){ // Reinitialiser le vecteur a des 0.
            maximum[i] = 0;
        }
}
	return(BPM);
}

volatile int Interface =0; 

volatile float dataIrBrut[1000];
volatile float* dataIRInterface;
volatile float dataRedBrut[1000];
volatile float* dataRedInterface;

float Saturation(float* dataIR, float* dataRed){
     NbreSec = NbreSec +1;
     for(int i = NbreSec*BLOCK_SIZE; i > (Interface*BLOCK_SIZE);i++){
        dataIrBrut[i] = dataIR[i];
        dataRedBrut[i] = dataRed[i];
    }
    Interface = Interface + 1; 
    if(Interface == 5){
        Interface =0;
        dataIRInterface = dataIrBrut;
        dataRedInterface = dataRedBrut;
    }
   
// Trouver les composantes DC
	float DCIR =0;
	float DCRED =0;
	arm_mean_f32(dataIR, BLOCK_SIZE,&DCIR);
	arm_mean_f32(dataRed, BLOCK_SIZE, &DCRED);

// Filtrage 
	float IrFiltre[250];
    float RedFiltre[250];
	arm_fir_init_f32(&S, FILTER_TAP_NUM, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);
    arm_fir_f32(&S, dataIR, IrFiltre,BLOCK_SIZE);
    arm_fir_f32(&S, dataRed, RedFiltre,BLOCK_SIZE);

// Trouver les composantes AC
	float maxIR =0;
	float maxRed =0;
	float minIR =0;
	float minRed =0;	
	uint32_t position =0;

	arm_max_f32(IrFiltre, BLOCK_SIZE, &maxIR, &position);
	arm_max_f32(RedFiltre, BLOCK_SIZE, &maxRed, &position);
	arm_min_f32(IrFiltre, BLOCK_SIZE, &minIR, &position);
	arm_min_f32(RedFiltre, BLOCK_SIZE, &minRed, &position);

	float ACIr = maxIR - minIR;
	float ACRed = maxRed - minRed;

// Trouver la saturation dans l'intervale:
	 R = (ACRed/DCRED)/(ACIr/DCIR);
	 SpO2 = -16.67*(R*R) + 8.33*R + 100;

// Trouver les maximums dans l'intervale
	for(int i=1; i>BLOCK_SIZE-1; i++){
		if((IrFiltre[i] > IrFiltre[i-1]) && (IrFiltre[i] > IrFiltre[i+1]) && (IrFiltre[i] >0)){
			Circulaire[NbreMax] = IrFiltre[i];
			NbreMax =NbreMax +1;
		}
	}

// Allumer les flags
	if(NbreSec % 20 == 0){
		NbreMax = 0;
		flag = 2;
	}
	if((NbreSec %10 == 0) && (NbreSec %20 !=0)){
		NbreMax = 30;
		flag = 1;
	}
    return(SpO2);
}
volatile bool Index = false;

// Valeurs a enlevees
float* maximum;
float* dataIR;
float* dataRed;

int main(void)
{
    SystemInit();
//    SystemCoreClockUpdate();
    
    xTaskCreate(AlarmeFC,"alarmeFC",80,NULL,1,NULL);
     
    vTaskStartScheduler();
  
    __enable_irq(); /* Enable global interrupts. */

    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        if(Index == true){
            Index = false;
            float SpO2 = Saturation(dataIR, dataRed);
        }
        if(NbreSec %10 == 0){           
            float bpm = BPM(maximum);
        }
        /* Place your application code here. */
        

    }
}




