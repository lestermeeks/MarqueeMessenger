// This #include statement was automatically added by the Particle IDE.
#include "MarqueeClient.h"



#include "Particle.h"
#include "softap_http.h"

// This #include statement was automatically added by the Particle IDE.
//#include <HttpClient.h>

// This #include statement was automatically added by the Particle IDE.
#include "GoldenTeeFrameBuffer.h"

// This #include statement was automatically added by the Particle IDE.
#include "GoldenTeeMarquee.h"






#if PLATFORM_ID == 6 // Photon
    #define PRODUCT_ID_VALUE 3623
#elif PLATFORM_ID == 10 // Electron
  #error "*** PLATFORM_ID not supported by this firmware. PLATFORM should be Core, Photon, P1 or Electron ***"
#endif

//electron project

#define PRODUCT_VERSION_VALUE 29
#define FIRMWARE_VERSION_VALUE "18040900"

PRODUCT_ID(PRODUCT_ID_VALUE);
PRODUCT_VERSION(PRODUCT_VERSION_VALUE);

SYSTEM_THREAD(ENABLED);

STARTUP(WiFi.setListenTimeout(60));
STARTUP(WiFi.selectAntenna(ANT_AUTO));


#define DEBUG_LED D7 
#define DEBUG_ON pinSetFast( DEBUG_LED )
#define DEBUG_OFF pinResetFast( DEBUG_LED )

#define TRANS_EN_PIN D0
#define TRANS_ON pinResetFast( TRANS_EN_PIN )
#define TRANS_OFF pinSetFast( TRANS_EN_PIN )



struct Page
{
    const char* url;
    const char* mime_type;
    const char* data;
};




// Headers currently need to be set at init, useful for API keys etc.
//http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    //{ "Accept" , "*/*"},
    //{ NULL, NULL } // NOTE: Always terminate headers will NULL
//};

marquee_request_t request;
//marquee_response_t response;

int messageCount =0, overflowCount=0, dupCount=0; 
bool fDebugState = false; 

unsigned long prevTicks = 0; 


// #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
//#define pgm_read_byte_near(addr) (*(const unsigned char *)(addr))

#define MESSAGE_SIZE 255
#define MESSAGE_STACK 10
char msgStack[MESSAGE_STACK][MESSAGE_SIZE+1];

#define PRIORITY_STACK 5
char priorityStack[PRIORITY_STACK][MESSAGE_SIZE+1];
//char msgTopNext[TOP_MESSAGE_SIZE+1];

//#define BTM_MESSAGE_SIZE 256 
//char msgBtm[BTM_MESSAGE_SIZE+1]; 
//char msgBtmNext[BTM_MESSAGE_SIZE+1];

#define DEBUG_MESSAGE_SIZE 255
char debugMsg[DEBUG_MESSAGE_SIZE+1] = "";

const int TOP_OFFSET_START = 71; 
int top_x_offset = TOP_OFFSET_START;
int top_y_offset;
int top_pixel_width;

const int BTM_OFFSET_START = 71; 
int btm_offset = BTM_OFFSET_START; 
int btm_pixel_width;

int TopBufferLength = 0; 
int BtmBufferLength = 0; 
char loopCount = 0;



#define MAX_MSG_LENGTH 255
char top_msg[MAX_MSG_LENGTH+1];
char btm_msg[MAX_MSG_LENGTH+1];
char msg_buff[MAX_MSG_LENGTH+1];
char get_buff[MAX_MSG_LENGTH+1] = {0};

int animateMs = 100;

//const int strFiltersCount = 2;
//const char* strFilters[strFiltersCount] = {"https://", "http://"};

const char* strDelim = "|||";
const char* settingDelim = ":";

// Pre-declare ISR callback functions
//void DoLedMachine(void);


MarqueeClient marqueeMessenger;
GoldenTeeMarquee *mySign;

ApplicationWatchdog wd(300000, System.reset);


const int LOCATION_SIZE = 63;
const int URL_SIZE = 127;
const int ENDPOINT_SIZE = 127;
typedef struct _device_settings_
{
    int size;
    //char location[LOCATION_SIZE+1];
    int ledPower;
    int refreshPerFrame;
    char messageServer[URL_SIZE+1];
    char messageEndpoint[URL_SIZE+1];
} DEVICE_SETTINGS;

DEVICE_SETTINGS deviceSettings;
String myIDStr;// = Spark.deviceID();
char request_url[MAX_MSG_LENGTH+1];



const char index_html[] = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><title>Setup your device</title><link rel='stylesheet' type='text/css' href='style.css'></head><body><h1>Connect me to your WiFi!</h1><h3>My device ID:</h3><input type=text id='device-id' size='25' value='' disabled/><button type='button' class='input-helper' id='copy-button'>Copy</button><div id='scan-div'><h3>Scan for visible WiFi networks</h3><button id='scan-button' type='button'>Scan</button></div><div id='networks-div'></div><div id='connect-div' style='display: none'><p>Don't see your network? Move me closer to your router, then re-scan.</p><form id='connect-form'><input type='password' id='password' size='25' placeholder='password'/><button type='button' class='input-helper' id='show-button'>Show</button><button type='submit' id='connect-button'>Connect</button></form></div><script src='rsa-utils/jsbn_1.js'></script><script src='rsa-utils/jsbn_2.js'></script><script src='rsa-utils/prng4.js'></script><script src='rsa-utils/rng.js'></script><script src='rsa-utils/rsa.js'></script><script src='script.js'></script></body></html>";

const char rsa_js[] = "function parseBigInt(a,b){return new BigInteger(a,b);}function linebrk(a,b){var c='';var d=0;while(d+b<a.length){c+=a.substring(d,d+b)+'\\n';d+=b;}return c+a.substring(d,a.length);}function byte2Hex(a){if(a<0x10)return '0'+a.toString(16);else return a.toString(16);}function pkcs1pad2(a,b){if(b<a.length+11){alert('Message too long for RSA');return null;}var c=new Array();var d=a.length-1;while(d>=0&&b>0){var e=a.charCodeAt(d--);if(e<128)c[--b]=e;else if((e>127)&&(e<2048)){c[--b]=(e&63)|128;c[--b]=(e>>6)|192;}else{c[--b]=(e&63)|128;c[--b]=((e>>6)&63)|128;c[--b]=(e>>12)|224;}}c[--b]=0;var f=new SecureRandom();var g=new Array();while(b>2){g[0]=0;while(g[0]==0)f.nextBytes(g);c[--b]=g[0];}c[--b]=2;c[--b]=0;return new BigInteger(c);}function RSAKey(){this.n=null;this.e=0;this.d=null;this.p=null;this.q=null;this.dmp1=null;this.dmq1=null;this.coeff=null;}function RSASetPublic(a,b){if(a!=null&&b!=null&&a.length>0&&b.length>0){this.n=parseBigInt(a,16);this.e=parseInt(b,16);}else alert('Invalid RSA public key');}function RSADoPublic(a){return a.modPowInt(this.e,this.n);}function RSAEncrypt(a){var b=pkcs1pad2(a,(this.n.bitLength()+7)>>3);if(b==null)return null;var c=this.doPublic(b);if(c==null)return null;var d=c.toString(16);if((d.length&1)==0)return d;else return '0'+d;}RSAKey.prototype.doPublic=RSADoPublic;RSAKey.prototype.setPublic=RSASetPublic;RSAKey.prototype.encrypt=RSAEncrypt;";

const char style_css[] = "html{height:100%;margin:auto;background-color:white}body{box-sizing:border-box;min-height:100%;padding:20px;background-color:#1aabe0;font-family:'Lucida Sans Unicode','Lucida Grande',sans-serif;font-weight:normal;color:white;margin-top:0;margin-left:auto;margin-right:auto;margin-bottom:0;max-width:400px;text-align:center;border:1px solid #6e6e70;border-radius:4px}div{margin-top:25px;margin-bottom:25px}h1{margin-top:25px;margin-bottom:25px}button{border-color:#1c75be;background-color:#1c75be;color:white;border-radius:5px;height:30px;font-size:15px;font-weight:bold}button.input-helper{background-color:#bebebe;border-color:#bebebe;color:#6e6e70;margin-left:3px}button:disabled{background-color:#bebebe;border-color:#bebebe;color:white}input[type='text'],input[type='password']{background-color:white;color:#6e6e70;border-color:white;border-radius:5px;height:25px;text-align:center;font-size:15px}input:disabled{background-color:#bebebe;border-color:#bebebe}input[type='radio']{position:relative;bottom:-0.33em;margin:0;border:0;height:1.5em;width:15%}label{padding-top:7px;padding-bottom:7px;padding-left:5%;display:inline-block;width:80%;text-align:left}input[type='radio']:checked+label{font-weight:bold;color:#1c75be}.scanning-error{font-weight:bold;text-align:center}.radio-div{box-sizing:border-box;margin:2px;margin-left:auto;margin-right:auto;background-color:white;color:#6e6e70;border:1px solid #6e6e70;border-radius:3px;width:100%;padding:5px}#networks-div{margin-left:auto;margin-right:auto;text-align:left}#device-id{text-align:center}#scan-button{min-width:100px}#connect-button{display:block;min-width:100px;margin-top:10px;margin-left:auto;margin-right:auto;margin-bottom:20px}#password{margin-top:20px;margin-bottom:10px}";

const char rng_js[] = "var rng_state;var rng_pool;var rng_pptr;function rng_seed_int(a){rng_pool[rng_pptr++]^=a&255;rng_pool[rng_pptr++]^=(a>>8)&255;rng_pool[rng_pptr++]^=(a>>16)&255;rng_pool[rng_pptr++]^=(a>>24)&255;if(rng_pptr>=rng_psize)rng_pptr-=rng_psize;}function rng_seed_time(){rng_seed_int(new Date().getTime());}if(rng_pool==null){rng_pool=new Array();rng_pptr=0;var t;if(window.crypto&&window.crypto.getRandomValues){var ua=new Uint8Array(32);window.crypto.getRandomValues(ua);for(t=0;t<32;++t)rng_pool[rng_pptr++]=ua[t];}if(navigator.appName=='Netscape'&&navigator.appVersion<'5'&&window.crypto){var z=window.crypto.random(32);for(t=0;t<z.length;++t)rng_pool[rng_pptr++]=z.charCodeAt(t)&255;}while(rng_pptr<rng_psize){t=Math.floor(65536*Math.random());rng_pool[rng_pptr++]=t>>>8;rng_pool[rng_pptr++]=t&255;}rng_pptr=0;rng_seed_time();}function rng_get_byte(){if(rng_state==null){rng_seed_time();rng_state=prng_newstate();rng_state.init(rng_pool);for(rng_pptr=0;rng_pptr<rng_pool.length;++rng_pptr)rng_pool[rng_pptr]=0;rng_pptr=0;}return rng_state.next();}function rng_get_bytes(a){var b;for(b=0;b<a.length;++b)a[b]=rng_get_byte();}function SecureRandom(){}SecureRandom.prototype.nextBytes=rng_get_bytes;";

const char jsbn_2_js[] = "function bnpRShiftTo(a,b){b.s=this.s;var c=Math.floor(a/this.DB);if(c>=this.t){b.t=0;return;}var d=a%this.DB;var e=this.DB-d;var f=(1<<d)-1;b[0]=this[c]>>d;for(var g=c+1;g<this.t;++g){b[g-c-1]|=(this[g]&f)<<e;b[g-c]=this[g]>>d;}if(d>0)b[this.t-c-1]|=(this.s&f)<<e;b.t=this.t-c;b.clamp();}function bnpSubTo(a,b){var c=0,d=0,e=Math.min(a.t,this.t);while(c<e){d+=this[c]-a[c];b[c++]=d&this.DM;d>>=this.DB;}if(a.t<this.t){d-=a.s;while(c<this.t){d+=this[c];b[c++]=d&this.DM;d>>=this.DB;}d+=this.s;}else{d+=this.s;while(c<a.t){d-=a[c];b[c++]=d&this.DM;d>>=this.DB;}d-=a.s;}b.s=(d<0)?-1:0;if(d<-1)b[c++]=this.DV+d;else if(d>0)b[c++]=d;b.t=c;b.clamp();}function bnpMultiplyTo(a,b){var c=this.abs(),d=a.abs();var e=c.t;b.t=e+d.t;while(--e>=0)b[e]=0;for(e=0;e<d.t;++e)b[e+c.t]=c.am(0,d[e],b,e,0,c.t);b.s=0;b.clamp();if(this.s!=a.s)BigInteger.ZERO.subTo(b,b);}function bnpSquareTo(a){var b=this.abs();var c=a.t=2*b.t;while(--c>=0)a[c]=0;for(c=0;c<b.t-1;++c){var d=b.am(c,b[c],a,2*c,0,1);if((a[c+b.t]+=b.am(c+1,2*b[c],a,2*c+1,d,b.t-c-1))>=b.DV){a[c+b.t]-=b.DV;a[c+b.t+1]=1;}}if(a.t>0)a[a.t-1]+=b.am(c,b[c],a,2*c,0,1);a.s=0;a.clamp();}function bnpDivRemTo(a,b,c){var d=a.abs();if(d.t<=0)return;var e=this.abs();if(e.t<d.t){if(b!=null)b.fromInt(0);if(c!=null)this.copyTo(c);return;}if(c==null)c=nbi();var f=nbi(),g=this.s,h=a.s;var i=this.DB-nbits(d[d.t-1]);if(i>0){d.lShiftTo(i,f);e.lShiftTo(i,c);}else{d.copyTo(f);e.copyTo(c);}var j=f.t;var k=f[j-1];if(k==0)return;var l=k*(1<<this.F1)+((j>1)?f[j-2]>>this.F2:0);var m=this.FV/l,n=(1<<this.F1)/l,o=1<<this.F2;var p=c.t,q=p-j,r=(b==null)?nbi():b;f.dlShiftTo(q,r);if(c.compareTo(r)>=0){c[c.t++]=1;c.subTo(r,c);}BigInteger.ONE.dlShiftTo(j,r);r.subTo(f,f);while(f.t<j)f[f.t++]=0;while(--q>=0){var s=(c[--p]==k)?this.DM:Math.floor(c[p]*m+(c[p-1]+o)*n);if((c[p]+=f.am(0,s,c,q,0,j))<s){f.dlShiftTo(q,r);c.subTo(r,c);while(c[p]<--s)c.subTo(r,c);}}if(b!=null){c.drShiftTo(j,b);if(g!=h)BigInteger.ZERO.subTo(b,b);}c.t=j;c.clamp();if(i>0)c.rShiftTo(i,c);if(g<0)BigInteger.ZERO.subTo(c,c);}function bnMod(a){var b=nbi();this.abs().divRemTo(a,null,b);if(this.s<0&&b.compareTo(BigInteger.ZERO)>0)a.subTo(b,b);return b;}function Classic(a){this.m=a;}function cConvert(a){if(a.s<0||a.compareTo(this.m)>=0)return a.mod(this.m);else return a;}function cRevert(a){return a;}function cReduce(a){a.divRemTo(this.m,null,a);}function cMulTo(a,b,c){a.multiplyTo(b,c);this.reduce(c);}function cSqrTo(a,b){a.squareTo(b);this.reduce(b);}Classic.prototype.convert=cConvert;Classic.prototype.revert=cRevert;Classic.prototype.reduce=cReduce;Classic.prototype.mulTo=cMulTo;Classic.prototype.sqrTo=cSqrTo;function bnpInvDigit(){if(this.t<1)return 0;var a=this[0];if((a&1)==0)return 0;var b=a&3;b=(b*(2-(a&0xf)*b))&0xf;b=(b*(2-(a&0xff)*b))&0xff;b=(b*(2-(((a&0xffff)*b)&0xffff)))&0xffff;b=(b*(2-a*b%this.DV))%this.DV;return(b>0)?this.DV-b:-b;}function Montgomery(a){this.m=a;this.mp=a.invDigit();this.mpl=this.mp&0x7fff;this.mph=this.mp>>15;this.um=(1<<(a.DB-15))-1;this.mt2=2*a.t;}function montConvert(a){var b=nbi();a.abs().dlShiftTo(this.m.t,b);b.divRemTo(this.m,null,b);if(a.s<0&&b.compareTo(BigInteger.ZERO)>0)this.m.subTo(b,b);return b;}function montRevert(a){var b=nbi();a.copyTo(b);this.reduce(b);return b;}function montReduce(a){while(a.t<=this.mt2)a[a.t++]=0;for(var b=0;b<this.m.t;++b){var c=a[b]&0x7fff;var d=(c*this.mpl+(((c*this.mph+(a[b]>>15)*this.mpl)&this.um)<<15))&a.DM;c=b+this.m.t;a[c]+=this.m.am(0,d,a,b,0,this.m.t);while(a[c]>=a.DV){a[c]-=a.DV;a[++c]++;}}a.clamp();a.drShiftTo(this.m.t,a);if(a.compareTo(this.m)>=0)a.subTo(this.m,a);}function montSqrTo(a,b){a.squareTo(b);this.reduce(b);}function montMulTo(a,b,c){a.multiplyTo(b,c);this.reduce(c);}Montgomery.prototype.convert=montConvert;Montgomery.prototype.revert=montRevert;Montgomery.prototype.reduce=montReduce;Montgomery.prototype.mulTo=montMulTo;Montgomery.prototype.sqrTo=montSqrTo;function bnpIsEven(){return((this.t>0)?(this[0]&1):this.s)==0;}function bnpExp(a,b){if(a>0xffffffff||a<1)return BigInteger.ONE;var c=nbi(),d=nbi(),e=b.convert(this),f=nbits(a)-1;e.copyTo(c);while(--f>=0){b.sqrTo(c,d);if((a&(1<<f))>0)b.mulTo(d,e,c);else{var g=c;c=d;d=g;}}return b.revert(c);}function bnModPowInt(a,b){var c;if(a<256||b.isEven())c=new Classic(b);else c=new Montgomery(b);return this.exp(a,c);}BigInteger.prototype.copyTo=bnpCopyTo;BigInteger.prototype.fromInt=bnpFromInt;BigInteger.prototype.fromString=bnpFromString;BigInteger.prototype.clamp=bnpClamp;BigInteger.prototype.dlShiftTo=bnpDLShiftTo;BigInteger.prototype.drShiftTo=bnpDRShiftTo;BigInteger.prototype.lShiftTo=bnpLShiftTo;BigInteger.prototype.rShiftTo=bnpRShiftTo;BigInteger.prototype.subTo=bnpSubTo;BigInteger.prototype.multiplyTo=bnpMultiplyTo;BigInteger.prototype.squareTo=bnpSquareTo;BigInteger.prototype.divRemTo=bnpDivRemTo;BigInteger.prototype.invDigit=bnpInvDigit;BigInteger.prototype.isEven=bnpIsEven;BigInteger.prototype.exp=bnpExp;BigInteger.prototype.toString=bnToString;BigInteger.prototype.negate=bnNegate;BigInteger.prototype.abs=bnAbs;BigInteger.prototype.compareTo=bnCompareTo;BigInteger.prototype.bitLength=bnBitLength;BigInteger.prototype.mod=bnMod;BigInteger.prototype.modPowInt=bnModPowInt;BigInteger.ZERO=nbv(0);BigInteger.ONE=nbv(1);";

const char jsbn_1_js[] = "var dbits;var canary=0xdeadbeefcafe;var j_lm=((canary&0xffffff)==0xefcafe);function BigInteger(a,b,c){if(a!=null)if('number'==typeof a)this.fromNumber(a,b,c);else if(b==null&&'string'!=typeof a)this.fromString(a,256);else this.fromString(a,b);}function nbi(){return new BigInteger(null);}function am1(a,b,c,d,e,f){while(--f>=0){var g=b*this[a++]+c[d]+e;e=Math.floor(g/0x4000000);c[d++]=g&0x3ffffff;}return e;}function am2(a,b,c,d,e,f){var g=b&0x7fff,h=b>>15;while(--f>=0){var i=this[a]&0x7fff;var j=this[a++]>>15;var k=h*i+j*g;i=g*i+((k&0x7fff)<<15)+c[d]+(e&0x3fffffff);e=(i>>>30)+(k>>>15)+h*j+(e>>>30);c[d++]=i&0x3fffffff;}return e;}function am3(a,b,c,d,e,f){var g=b&0x3fff,h=b>>14;while(--f>=0){var i=this[a]&0x3fff;var j=this[a++]>>14;var k=h*i+j*g;i=g*i+((k&0x3fff)<<14)+c[d]+e;e=(i>>28)+(k>>14)+h*j;c[d++]=i&0xfffffff;}return e;}if(j_lm&&(navigator.appName=='Microsoft Internet Explorer')){BigInteger.prototype.am=am2;dbits=30;}else if(j_lm&&(navigator.appName!='Netscape')){BigInteger.prototype.am=am1;dbits=26;}else{BigInteger.prototype.am=am3;dbits=28;}BigInteger.prototype.DB=dbits;BigInteger.prototype.DM=((1<<dbits)-1);BigInteger.prototype.DV=(1<<dbits);var BI_FP=52;BigInteger.prototype.FV=Math.pow(2,BI_FP);BigInteger.prototype.F1=BI_FP-dbits;BigInteger.prototype.F2=2*dbits-BI_FP;var BI_RM='0123456789abcdefghijklmnopqrstuvwxyz';var BI_RC=new Array();var rr,vv;rr='0'.charCodeAt(0);for(vv=0;vv<=9;++vv)BI_RC[rr++]=vv;rr='a'.charCodeAt(0);for(vv=10;vv<36;++vv)BI_RC[rr++]=vv;rr='A'.charCodeAt(0);for(vv=10;vv<36;++vv)BI_RC[rr++]=vv;function int2char(a){return BI_RM.charAt(a);}function intAt(a,b){var c=BI_RC[a.charCodeAt(b)];return(c==null)?-1:c;}function bnpCopyTo(a){for(var b=this.t-1;b>=0;--b)a[b]=this[b];a.t=this.t;a.s=this.s;}function bnpFromInt(a){this.t=1;this.s=(a<0)?-1:0;if(a>0)this[0]=a;else if(a<-1)this[0]=a+this.DV;else this.t=0;}function nbv(a){var b=nbi();b.fromInt(a);return b;}function bnpFromString(a,b){var c;if(b==16)c=4;else if(b==8)c=3;else if(b==256)c=8;else if(b==2)c=1;else if(b==32)c=5;else if(b==4)c=2;else{this.fromRadix(a,b);return;}this.t=0;this.s=0;var d=a.length,e=false,f=0;while(--d>=0){var g=(c==8)?a[d]&0xff:intAt(a,d);if(g<0){if(a.charAt(d)=='-')e=true;continue;}e=false;if(f==0)this[this.t++]=g;else if(f+c>this.DB){this[this.t-1]|=(g&((1<<(this.DB-f))-1))<<f;this[this.t++]=(g>>(this.DB-f));}else this[this.t-1]|=g<<f;f+=c;if(f>=this.DB)f-=this.DB;}if(c==8&&(a[0]&0x80)!=0){this.s=-1;if(f>0)this[this.t-1]|=((1<<(this.DB-f))-1)<<f;}this.clamp();if(e)BigInteger.ZERO.subTo(this,this);}function bnpClamp(){var a=this.s&this.DM;while(this.t>0&&this[this.t-1]==a)--this.t;}function bnToString(a){if(this.s<0)return '-'+this.negate().toString(a);var b;if(a==16)b=4;else if(a==8)b=3;else if(a==2)b=1;else if(a==32)b=5;else if(a==4)b=2;else return this.toRadix(a);var c=(1<<b)-1,d,e=false,f='',g=this.t;var h=this.DB-(g*this.DB)%b;if(g-->0){if(h<this.DB&&(d=this[g]>>h)>0){e=true;f=int2char(d);}while(g>=0){if(h<b){d=(this[g]&((1<<h)-1))<<(b-h);d|=this[--g]>>(h+=this.DB-b);}else{d=(this[g]>>(h-=b))&c;if(h<=0){h+=this.DB;--g;}}if(d>0)e=true;if(e)f+=int2char(d);}}return e?f:'0';}function bnNegate(){var a=nbi();BigInteger.ZERO.subTo(this,a);return a;}function bnAbs(){return(this.s<0)?this.negate():this;}function bnCompareTo(a){var b=this.s-a.s;if(b!=0)return b;var c=this.t;b=c-a.t;if(b!=0)return(this.s<0)?-b:b;while(--c>=0)if((b=this[c]-a[c])!=0)return b;return 0;}function nbits(a){var b=1,c;if((c=a>>>16)!=0){a=c;b+=16;}if((c=a>>8)!=0){a=c;b+=8;}if((c=a>>4)!=0){a=c;b+=4;}if((c=a>>2)!=0){a=c;b+=2;}if((c=a>>1)!=0){a=c;b+=1;}return b;}function bnBitLength(){if(this.t<=0)return 0;return this.DB*(this.t-1)+nbits(this[this.t-1]^(this.s&this.DM));}function bnpDLShiftTo(a,b){var c;for(c=this.t-1;c>=0;--c)b[c+a]=this[c];for(c=a-1;c>=0;--c)b[c]=0;b.t=this.t+a;b.s=this.s;}function bnpDRShiftTo(a,b){for(var c=a;c<this.t;++c)b[c-a]=this[c];b.t=Math.max(this.t-a,0);b.s=this.s;}function bnpLShiftTo(a,b){var c=a%this.DB;var d=this.DB-c;var e=(1<<d)-1;var f=Math.floor(a/this.DB),g=(this.s<<c)&this.DM,h;for(h=this.t-1;h>=0;--h){b[h+f+1]=(this[h]>>d)|g;g=(this[h]&e)<<c;}for(h=f-1;h>=0;--h)b[h]=0;b[f]=g;b.t=this.t+f+1;b.s=this.s;b.clamp();}";

const char script_js[] = "var base_url='http://192.168.0.1/';var network_list;var public_key;var rsa=new RSAKey();var scanButton=document.getElementById('scan-button');var connectButton=document.getElementById('connect-button');var copyButton=document.getElementById('copy-button');var showButton=document.getElementById('show-button');var deviceID=document.getElementById('device-id');var connectForm=document.getElementById('connect-form');var public_key_callback={success:function(a){console.log('Public key: '+a.b);public_key=a.b;rsa.setPublic(public_key.substring(58,58+256),public_key.substring(318,318+6));},error:function(a,b){console.log(a);window.alert('There was a problem fetching important information from your device. Please verify your connection, then reload this page.');}};var device_id_callback={success:function(a){var b=a.id;deviceID.value=b;},error:function(a,b){console.log(a);var c='COMMUNICATION_ERROR';deviceID.value=c;}};var scan=function(){console.log('Scanning...!');disableButtons();scanButton.innerHTML='Scanning...';connectButton.innerHTML='Connect';document.getElementById('connect-div').style.display='none';document.getElementById('networks-div').style.display='none';getRequest(base_url+'scan-ap',scan_callback);};var scan_callback={success:function(a){network_list=a.scans;console.log('I found:');var b=document.getElementById('networks-div');b.innerHTML='';if(network_list.length>0)for(var c=0;c<network_list.length;c++){ssid=network_list[c].ssid;console.log(network_list[c]);add_wifi_option(b,ssid);document.getElementById('connect-div').style.display='block';}else b.innerHTML='<p class=\\'scanning-error\\'>No networks found.</p>';},error:function(a){console.log('Scanning error:'+a);document.getElementById('networks-div').innerHTML='<p class=\\'scanning-error\\'>Scanning error.</p>';},regardless:function(){scanButton.innerHTML='Re-Scan';enableButtons();document.getElementById('networks-div').style.display='block';}};var configure=function(a){a.preventDefault();var b=get_selected_network();var c=document.getElementById('password').value;if(!b){window.alert('Please select a network!');return false;}var d={idx:0,ssid:b.ssid,pwd:rsa.encrypt(c),sec:b.sec,ch:b.ch};connectButton.innerHTML='Sending credentials...';disableButtons();console.log('Sending credentials: '+JSON.stringify(d));postRequest(base_url+'configure-ap',d,configure_callback);};var configure_callback={success:function(a){console.log('Credentials received.');connectButton.innerHTML='Credentials received...';postRequest(base_url+'connect-ap',{idx:0},connect_callback);},error:function(a,b){console.log('Configure error: '+a);window.alert('The configuration command failed, check that you are still well connected to the device\\'s WiFi hotspot and retry.');connectButton.innerHTML='Retry';enableButtons();}};var connect_callback={success:function(a){console.log('Attempting to connect to the cloud.');connectButton.innerHTML='Attempting to connect...';window.alert('Your device should now start flashing green and attempt to connect to the cloud. This usually takes about 20 seconds, after which it will begin slowly blinking cyan. \\n\\n\\nIf this process fails because you entered the wrong password, the device will flash green indefinitely. In this case, hold the setup button for 6 seconds until the device starts blinking blue again. Then reconnect to the WiFi hotspot it generates and reload this page to try again.');},error:function(a,b){console.log('Connect error: '+a);window.alert('The connect command failed, check that you are still well connected to the device\\'s WiFi hotspot and retry.');connectButton.innerHTML='Retry';enableButtons();}};var disableButtons=function(){connectButton.disabled=true;scanButton.disabled=true;};var enableButtons=function(){connectButton.disabled=false;scanButton.disabled=false;};var add_wifi_option=function(a,b){var c=document.createElement('INPUT');c.type='radio';c.value=b;c.id=b;c.name='ssid';c.className='radio';var d=document.createElement('DIV');d.className='radio-div';d.appendChild(c);var e=document.createElement('label');e.htmlFor=b;e.innerHTML=b;d.appendChild(e);a.appendChild(d);};var get_selected_network=function(){for(var a=0;a<network_list.length;a++){ssid=network_list[a].ssid;if(document.getElementById(ssid).checked)return network_list[a];}};var copy=function(){window.prompt('Copy to clipboard: Ctrl + C, Enter',deviceID.value);};var toggleShow=function(){var a=document.getElementById('password');inputType=a.type;if(inputType==='password'){showButton.innerHTML='Hide';a.type='text';}else{showButton.innerHTML='Show';a.type='password';}};var getRequest=function(a,b){var c=new XMLHttpRequest();c.open('GET',a,true);c.timeout=8000;c.send();c.onreadystatechange=function(){if(c.readyState==4)if(b){if(c.status==200){if(b.success)b.success(JSON.parse(c.responseText));}else if(b.error)b.error(c.status,c.responseText);if(b.regardless)b.regardless();}};};var postRequest=function(a,b,c){var d=JSON.stringify(b);var e=new XMLHttpRequest();e.open('POST',a,true);e.timeout=4000;e.setRequestHeader('Content-Type','multipart/form-data');e.send(d);e.onreadystatechange=function(){if(e.readyState==4)if(c){if(e.status==200){if(c.success)c.success(JSON.parse(e.responseText));}else if(c.error)c.error(e.status,e.responseText);if(c.regardless)c.regardless();}};};if(scanButton.addEventListener){copyButton.addEventListener('click',copy);showButton.addEventListener('click',toggleShow);scanButton.addEventListener('click',scan);connectForm.addEventListener('submit',configure);}else if(scanButton.attachEvent){copyButton.attachEvent('onclick',copy);showButton.attachEvent('onclick',toggleShow);scanButton.attachEvent('onclick',scan);connectForm.attachEvent('onsubmit',configure);}getRequest(base_url+'device-id',device_id_callback);getRequest(base_url+'public-key',public_key_callback);";

const char prng4_js[] = "function Arcfour(){this.i=0;this.j=0;this.S=new Array();}function ARC4init(a){var b,c,d;for(b=0;b<256;++b)this.S[b]=b;c=0;for(b=0;b<256;++b){c=(c+this.S[b]+a[b%a.length])&255;d=this.S[b];this.S[b]=this.S[c];this.S[c]=d;}this.i=0;this.j=0;}function ARC4next(){var a;this.i=(this.i+1)&255;this.j=(this.j+this.S[this.i])&255;a=this.S[this.i];this.S[this.i]=this.S[this.j];this.S[this.j]=a;return this.S[(a+this.S[this.i])&255];}Arcfour.prototype.init=ARC4init;Arcfour.prototype.next=ARC4next;function prng_newstate(){return new Arcfour();}var rng_psize=256;";

Page myPages[] = {
     { "/index.html", "text/html", index_html },
     { "/rsa-utils/rsa.js", "application/javascript", rsa_js },
     { "/style.css", "text/css", style_css },
     { "/rsa-utils/rng.js", "application/javascript", rng_js },
     { "/rsa-utils/jsbn_2.js", "application/javascript", jsbn_2_js },
     { "/rsa-utils/jsbn_1.js", "application/javascript", jsbn_1_js },
     { "/script.js", "application/javascript", script_js },
     { "/rsa-utils/prng4.js", "application/javascript", prng4_js },
     { nullptr }
};

void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
    Serial.printlnf("handling page %s", url);

    if (strcmp(url,"/index")==0) {
        Serial.println("sending redirect");
        Header h("Location: /index.html\r\n");
        cb(cbArg, 0, 301, "text/plain", &h);
        return;
    }

    int8_t idx = 0;
    for (;;idx++) {
        Page& p = myPages[idx];
        if (!p.url) {
            idx = -1;
            break;
        }
        else if (strcmp(url, p.url)==0) {
            break;
        }
    }

    if (idx==-1) {
        cb(cbArg, 0, 404, nullptr, nullptr);
    }
    else {
        cb(cbArg, 0, 200, myPages[idx].mime_type, nullptr);
        result->write(myPages[idx].data);
    }
}

STARTUP(softap_set_application_page_handler(myPage, nullptr));





bool splitMessage(const char *message, const char* delim, char *first, int max_first, char *second, int max_second)
{
    //char* retval = NULL;
    
    if(message != NULL)
    {
        int messageLength = strlen(message);
        int delimLength = strlen(delim);
        char* token = strstr( message, delim);
        
        if(token > message)
        {
            //we found a token somewher inside the message.
            if(token - message + delimLength < messageLength)
            {
                memset(first, 0, max_first);
                memset(second, 0, max_second);
                //token[0] = '\0';
                //we have a token not at the end either...  split
                //retval = token + strlen(strDelim);
                //*token = 0;
                int firstLength = token-message;
                if(firstLength > max_first)
                    firstLength = max_first;
                
                strncpy(first, message, firstLength);
                strncpy(second, token + delimLength, max_second);
                return true;
            }
        }
        
    }
    
    return false;
}


//int applyFilters(String msg)
//{
//    for(int filterIdx = 0; filterIdx < strFiltersCount; filterIdx++)
//    {
//        if(strstr(msg, strFilters[filterIdx]) != 0)
//        {
//            return filterIdx;
//        }
//    }
//    return -1;
//}

int priorityMessageWithHeader(const char* first, const char* second)
{
    char temp[MESSAGE_SIZE] = {0};
    strncpy(temp, first, MESSAGE_SIZE);
    strncat(temp, strDelim, MESSAGE_SIZE);
    strncat(temp, second, MESSAGE_SIZE);
    return priorityMessage(temp);
    
}

int priorityMessage(String msg)
{
    char temp[MESSAGE_SIZE] = {0};
    strncpy(temp, msg, MESSAGE_SIZE);

    for(int stackIdx = 0; stackIdx < PRIORITY_STACK; stackIdx++)
    {
        if(priorityStack[stackIdx][0] == '\0')
        {
            //messageCount++;
            memset(priorityStack[stackIdx], 0, MESSAGE_SIZE);
            strncpy(priorityStack[stackIdx], temp, MESSAGE_SIZE);
            return stackIdx;
        }
    }

    return -1;
}


//settings stuff
const int SETTING_NAME_MAX = 12;
const int SETTING_VALUE_MAX = 48;
enum SETTING_ID
{
    ANIMATE_TIME,
    LED_POWER,
    FRAME_COUNT,
    LOCATION,
    SERVER,
    SETTING_ID_RESET,
};





typedef bool (*SettingHandlerFn)(char* value);
typedef struct _setting_desc_
{
    SETTING_ID id;
    char name[SETTING_NAME_MAX+1];
    SettingHandlerFn handler;
    
} SETTING_DESC;

bool HandleLedPower(char* value)
{
    int iValue = atoi(value);
    if(iValue > 0)
    {
        deviceSettings.ledPower = iValue;
        mySign->SetBitOffIndex((byte)iValue);
        EEPROM.put(0, deviceSettings);
    }   
    return true;
}

bool HandleFrameCount(char* value){
    int iValue = atoi(value);
    if(iValue > 0)
    {
        deviceSettings.refreshPerFrame = iValue;
        mySign->SetRefreshPerFrame((byte)iValue);
        EEPROM.put(0, deviceSettings);
        return true;
    }   
    return false;
}

bool HandleReset(char* value)
{
    if (strcmp(value, "settings") ==0)
    {
        //WiFi.clearCredentials();
        EEPROM.clear();
        return true;
        //System.reset();
    }
    else if (strcmp(value, "wifi") ==0)
    {
        WiFi.clearCredentials();
        return true;
        //EEPROM.clear();
        //System.reset();
    }
    else
    {
        System.reset();
        return true;
    }
    return false;
}

const int KNOWN_SETTINGS_COUNT = 3;
const SETTING_DESC KNOWN_SETTINGS[KNOWN_SETTINGS_COUNT] = {
  //{ ANIMATE_TIME, "animateMS", HandleAnimateTime },
  { LED_POWER, "ledPower", HandleLedPower},
  { FRAME_COUNT, "frameCount", HandleFrameCount},
  //{ SERVER, "server", HandleServer},
  { SETTING_ID_RESET, "reset", HandleReset},
};

int pushSetting(String setting)
{
    char temp[MESSAGE_SIZE] = {0};
    char name[SETTING_NAME_MAX], value[SETTING_VALUE_MAX];
    strncpy(temp, setting, MESSAGE_SIZE);
    //priorityMessage("Setting Received");
    if(splitMessage(temp, settingDelim, name, SETTING_NAME_MAX, value, SETTING_VALUE_MAX))
    {
        priorityMessageWithHeader(name, value);

        bool found = false;
        for( int handlerIdx = 0; handlerIdx < KNOWN_SETTINGS_COUNT; handlerIdx++)
        {
            if(strcmp(name, KNOWN_SETTINGS[handlerIdx].name) == 0)
            {
                found = true;
                bool handled = (KNOWN_SETTINGS[handlerIdx].handler(value));
                if(!handled)
                {
                    //not a known setting?
                    priorityMessageWithHeader("Failed Setting", temp);
                    //priorityMessage(setting);
                }
                
            }

        }
        
        if(found)
        {
            return 1;
        }
        else
        {
            priorityMessageWithHeader("Unknown Setting", temp);
            return 0;
            //priorityMessage(setting);
        }
        
    }
    else
    {
        priorityMessageWithHeader("Malformed Setting", temp);
        return -1;
        //priorityMessage(setting);
    }
    
    //return 1;
}

int pushMessage(String msg)
{

    for(int stackIdx = 0; stackIdx < MESSAGE_STACK; stackIdx++)
    {
        if(msgStack[stackIdx][0] == '\0')
        {
            messageCount++;
            memset(msgStack[stackIdx], 0, MESSAGE_SIZE);
            strncpy(msgStack[stackIdx], msg, MESSAGE_SIZE);
            return stackIdx;
        }
    }

    overflowCount++;
    return -1;
}

int MessageStackLevel()
{
    for(int stackIdx = 0; stackIdx < MESSAGE_STACK; stackIdx++)
    {
        if(msgStack[stackIdx][0] == '\0')
        {
            return stackIdx;
        }
    }

    return MESSAGE_STACK;
}
int pushMessageWithHeader(const char* first, const char* second)
{
    char temp[MESSAGE_SIZE] = {0};
    strncpy(temp, first, MESSAGE_SIZE);
    strncat(temp, strDelim, MESSAGE_SIZE);
    strncat(temp, second, MESSAGE_SIZE);
    return pushMessage(temp);
    
}

void pushMessageHandler(const char *topic, const char *data)
{
    pushMessage( data );
}

void HandleRequestSuccess(MarqueeClient *client, marquee_request_t* request, char* body){
    if(strlen(body) > 0)
    {
        pushMessage(body);
        //Particle.publish("/req/success", body);
    }
    else
    {
        Particle.publish("/req/empty", body);
    }
}
void HandleRequestTimeout(MarqueeClient *client, marquee_request_t* request, char* body){
    Particle.publish("/req/timeout", body);
}

void HandleRequestError(MarqueeClient *client, marquee_request_t* request, char* body){
    Particle.publish("/req/error", body);
}

void HandleRequestComplete(MarqueeClient *client, marquee_request_t* request, char* body){
    //snprintf(msg_buff, MAX_MSG_LENGTH, "Last Elapsed: %d", client->GetElapsedTicks() );
    //Particle.publish("/req/complete", msg_buff);
}


void setup(void) {
   
    Serial.begin(115200); 
    Serial.println("Setup ++");
    
    marqueeMessenger.SetReceiveBuffer(get_buff, MAX_MSG_LENGTH);
    marqueeMessenger.SetRequestHandlers( HandleRequestSuccess, HandleRequestTimeout, HandleRequestError, HandleRequestComplete);
    
    pinMode(DEBUG_LED, OUTPUT);		// Configure LED pins as OUTPUTs

    snprintf(msg_buff, MAX_MSG_LENGTH, "V%u (%s)", PRODUCT_VERSION_VALUE, FIRMWARE_VERSION_VALUE );
    priorityMessageWithHeader("Version:", msg_buff);

    WiFiAccessPoint ap[5];

    int wifiCredsCount = WiFi.getCredentials(ap, 5);
    
    for(int credsIdx = 0; credsIdx < wifiCredsCount; credsIdx++)
    {
        //strncpy(msg_buff, "SSID:|||", MAX_MSG_LENGTH);
        //strncat(msg_buff, ap[credsIdx].ssid, MAX_MSG_LENGTH);
        priorityMessageWithHeader("SSID", ap[credsIdx].ssid );
        //setMsgTop("ssid: ");
        //setMsgTop(ap[0].ssid);
        // security is one of WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA, WLAN_SEC_WPA2
        //Serial.print("security: ");
        //Serial.println(ap[0].security);
        // cipher is one of WLAN_CIPHER_AES, WLAN_CIPHER_TKIP
        //Serial.print("cipher: ");
        //Serial.println(ap[0].cipher);
    }
    
    Particle.function("pushMsg", pushMessage);
    Particle.function("priorityMsg", priorityMessage);
    Particle.function("pushCmd", pushSetting);
    //Particle.subscribe("goldentee/tweet/", pushMessageHandler);

    EEPROM.get(0, deviceSettings);
    if(deviceSettings.size != sizeof(deviceSettings)) {
         pushMessage("EEPROM|||Default Settings");
         deviceSettings.size = sizeof(deviceSettings);
         deviceSettings.ledPower = 50;
         deviceSettings.refreshPerFrame = 10;
         strncpy(deviceSettings.messageServer, "dashboard.marqueemessenger.com", URL_SIZE);
         strncpy(deviceSettings.messageEndpoint, "/api/device/%s/message", URL_SIZE);
    }
    
    if(EEPROM.hasPendingErase()) {
        EEPROM.performPendingErase();
    }


    myIDStr = Spark.deviceID();
    strncpy(request_url, "/api/device/", MAX_MSG_LENGTH);
    strncat(request_url, myIDStr, MAX_MSG_LENGTH);
    strncat(request_url, "/message", MAX_MSG_LENGTH);
    //sprintf(request_url, MAX_MSG_LENGTH, deviceSettings.messageEndpoint, myIDStr);
    
    pushMessageWithHeader("settings:host",deviceSettings.messageServer);
    pushMessageWithHeader("settings:path", request_url);

    //get the values.
    //http://node-golden-dashboard.herokuapp.com/api/device/deviceid/message
    request.hostname = deviceSettings.messageServer;
    //request.port = 80;
    request.path = request_url;
    
    //init our sign and its basic settings.
    mySign = GoldenTeeMarquee::Create();
    mySign->SetBitOffIndex((byte)deviceSettings.ledPower);
    mySign->SetRefreshPerFrame((byte) deviceSettings.refreshPerFrame);

    //turn on our level shifter
    pinMode(TRANS_EN_PIN, OUTPUT);
    TRANS_ON;
        

}



#define TOP_ROW_Y (0 * 7)
#define BTM_ROW_Y (1 * 7)
bool readyForNext = true;
int requestFailCount = 0;
const int REQUEST_FAIL_LIMIT = 5;

void loop(void) {
    //fDebugState = !fDebugState; 
    //fDebugState?DEBUG_ON:DEBUG_OFF; 

    unsigned long currentTicks = millis();
    
    
    // if the sign has a free buffer, grab it and render a new frame into it.
    // for now we are running the main loop at FULL speed, any time a frame buffer
    // is ready to use, we do it.
    GoldenTeeFrameBuffer* writeBuffer = mySign->GetFrameBuffer();
        
    if(writeBuffer == NULL)
    {
        MARQUEE_RESPONSE_STATE state = marqueeMessenger.CheckRequest( );
        
        if(state == RESPONSE_STATE_UNKNOWN && WiFi.ready() && MessageStackLevel() < MESSAGE_STACK/2)
        {
            //snprintf(msg_buff, MAX_MSG_LENGTH, "Last Elapsed: %d", marqueeMessenger.GetElapsedTicks() );
            
            if(marqueeMessenger.StartRequest( request.hostname, request.path ))
            {
                //Particle.publish("/req/start/failed", msg_buff);
            }
            else
            {
                Particle.publish("/req/start/failed", msg_buff);
            }

        }
        
        if(state > RESPONSE_STATE_UNKNOWN)
            DEBUG_ON;
        else
            DEBUG_OFF;
                
    }
   else
    //if( writeBuffer )//  && (currentTicks - prevTicks >= animateMs)) 
    {
        //bool listening = Wifi.listening();
        //memset(writeBuffer, 0, LED_ROW_ENABLE_COUNT*2 * LED_ROW_BYTE_COUNT);
        writeBuffer->Clear();

        
        //writeBuffer->CopyStringToFrameBuffer( "A", 0, TOP_ROW_Y); 
        
        
        if(readyForNext)
        {
            //GoldenTeeMarquee::Pause();
            
            wd.checkin();
            fDebugState = !fDebugState; 
            //if(fDebugState)
            //{
            //    DEBUG_ON; 
            //}
            //else 
            //{
            //    DEBUG_OFF; 
            //}
            
            readyForNext = false;
                        
            memset(top_msg, 0, MAX_MSG_LENGTH);
            memset(btm_msg, 0, MAX_MSG_LENGTH);

            if(strlen(priorityStack[0]))
            {
                requestFailCount = 0;
                if(!splitMessage(priorityStack[0], strDelim, top_msg, MAX_MSG_LENGTH, btm_msg, MAX_MSG_LENGTH))
                {
                    //put the message in the middle?
                    //strncpy(top_msg, msgTop[0], MAX_MSG_LENGTH);
                    //TopBufferLength = 0;
                    //top_msg[0] = '\0';
                    strncpy(top_msg, priorityStack[0], MAX_MSG_LENGTH);
                    strncpy(btm_msg, "", MAX_MSG_LENGTH);
                }

                //roll the stack up one and check for a message
                for(int msgIdx = 0; msgIdx < PRIORITY_STACK-1; msgIdx++)
                {
                    strncpy(priorityStack[msgIdx], priorityStack[msgIdx+1], MESSAGE_SIZE);
                }
                
                memset(priorityStack[PRIORITY_STACK-1], 0, MESSAGE_SIZE);
                
            }
            else if(strlen(msgStack[0]))
            {
                requestFailCount = 0;
                if(!splitMessage(msgStack[0], strDelim, top_msg, MAX_MSG_LENGTH, btm_msg, MAX_MSG_LENGTH))
                {
                    //put the message in the middle?
                    //strncpy(top_msg, msgTop[0], MAX_MSG_LENGTH);
                    //TopBufferLength = 0;
                    //top_msg[0] = '\0';
                    strncpy(top_msg, msgStack[0], MAX_MSG_LENGTH);
                    strncpy(btm_msg, "", MAX_MSG_LENGTH);
                }

                //roll the stack up one and check for a message
                for(int msgIdx = 0; msgIdx < MESSAGE_STACK-1; msgIdx++)
                {
                    strncpy(msgStack[msgIdx], msgStack[msgIdx+1], MESSAGE_SIZE);
                }
                
                memset(msgStack[MESSAGE_STACK-1], 0, MESSAGE_SIZE);

            }
            else
            {
                if(WiFi.ready())// && Particle.connected())
                {
                    requestFailCount++;
                    
                    //pushMessage("Particle Cloud|||NOT Connected");
                    strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                    //snprintf(msg_buff, MAX_MSG_LENGTH, "V%u (%s)", PRODUCT_VERSION_VALUE, FIRMWARE_VERSION_VALUE );
   
                    snprintf(btm_msg, MAX_MSG_LENGTH , "Idle ... %d", requestFailCount );
                    Particle.publish("/sys/idle", btm_msg);
                    
                    
                    if(requestFailCount > 10)
                    {
                        System.reset();
                    }
/*
                    
                    switch(state)
                    {
                        case RESPONSE_STATE_UNKNOWN:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "RESPONSE_STATE_UNKNOWN", MAX_MSG_LENGTH);
                            //Particle.publish("/req/state", btm_msg);
                            break;
                        case RESPONSE_STATE_OPEN:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "RESPONSE_STATE_OPEN", MAX_MSG_LENGTH);
                            //Particle.publish("/req/state", btm_msg);
                            break;
                        case RESPONSE_STATE_HEADERS:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "RESPONSE_STATE_HEADERS", MAX_MSG_LENGTH);
                            //Particle.publish("/req/state", btm_msg);
                            break;
                        case RESPONSE_STATE_DATA:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "RESPONSE_STATE_DATA", MAX_MSG_LENGTH);
                            //Particle.publish("/req/state", btm_msg);
                            break;
                            
                        case RESPONSE_STATE_COMPLETE:
                            //strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            //strncpy(btm_msg, "RESPONSE_STATE_COMPLETE", MAX_MSG_LENGTH);
                            //Particle.publish("/req/state", "RESPONSE_STATE_COMPLETE");
                            break;
                        case RESPONSE_STATE_TIMEOUT:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "RESPONSE_STATE_TIMEOUT", MAX_MSG_LENGTH);
                            Particle.publish("/req/state", "RESPONSE_STATE_TIMEOUT");
                            break;
                        case RESPONSE_STATE_OVERFLOW:
                            //strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            //strncpy(btm_msg, "RESPONSE_STATE_OVERFLOW", MAX_MSG_LENGTH);
                            Particle.publish("/req/state", "RESPONSE_STATE_OVERFLOW");
                            break;
                        default:
                            strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                            strncpy(btm_msg, "Unknown Response State", MAX_MSG_LENGTH);
                            Particle.publish("/req/state", "RESPONSE_STATE_UNHANDLED");
                            break;
                            
                        
                    }
*/

                    
                    //response.body = "";
                    //DEBUG_ON;
                    //http.get(request, response);//, headers);
                    
                    //DEBUG_OFF;
                    //request failed, got an empty response?
                    /*
                    if(response.status == -1 || strlen(response.body) == 0)
                    {
                        snprintf(msg_buff, MAX_MSG_LENGTH, "Code: %d", response.status );
                        Particle.publish("get_fail", msg_buff);

                        strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                        strncpy(btm_msg, msg_buff, MAX_MSG_LENGTH);
                        
                        if(++requestFailCount >= REQUEST_FAIL_LIMIT)
                        {
                            
                            
                            //TRANS_OFF;
                            System.reset();
                        }
                        
                    }
                    else
                    {
                        requestFailCount = 0;
                        //pushMessage("Particle Cloud|||Connected");
                        if(!splitMessage(response.body, strDelim, top_msg, MAX_MSG_LENGTH, btm_msg, MAX_MSG_LENGTH))
                        {
                            //put the message in the middle?
                            //strncpy(top_msg, msgTop[0], MAX_MSG_LENGTH);
                            //TopBufferLength = 0;
                            //top_msg[0] = '\0';
                            strncpy(top_msg, response.body, MAX_MSG_LENGTH);
                            strncpy(btm_msg, "", MAX_MSG_LENGTH);
                        }
                    }
                    */
                }
                else if(WiFi.connecting())
                {
                    //pushMessage("Particle Cloud|||NOT Connected");
                    strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                    strncpy(btm_msg, "WiFi Connecting...", MAX_MSG_LENGTH);
                }
                else if(WiFi.listening())
                {
                    //pushMessage("Particle Cloud|||NOT Connected");
                    strncpy(top_msg, " - Setup - ", MAX_MSG_LENGTH);
                    strncpy(btm_msg, "Waiting for setup on device wifi network - http://192.168.0.1", MAX_MSG_LENGTH);
                }
                else 
                {
                    //not listening but not ready...
                                        //pushMessage("Particle Cloud|||NOT Connected");
                    strncpy(top_msg, " - Status - ", MAX_MSG_LENGTH);
                    strncpy(btm_msg, "Wifi not connected.", MAX_MSG_LENGTH);
                    
                    
                }
            
                
            }
            
            
            TopBufferLength = strlen(top_msg);
            BtmBufferLength = strlen(btm_msg);
            
            
            if(BtmBufferLength > 0)
            {
                //top slide down, bottom scroll, both scale 1
                top_pixel_width = writeBuffer->MeasureStringVar(top_msg, 1);
                btm_pixel_width = writeBuffer->MeasureStringVar(btm_msg, 1);
                
                btm_offset = BTM_OFFSET_START;
                top_x_offset = writeBuffer->GetPixelWidth()/2 - top_pixel_width/2;
                top_y_offset = -7;
            }
            
            else
            {
                //top scroll, scale 2
                top_pixel_width = writeBuffer->MeasureStringVar(top_msg, 2);
                btm_pixel_width = 0;
                
                btm_offset = BTM_OFFSET_START;
                top_x_offset = TOP_OFFSET_START;
                top_y_offset = 0;
            }
            
            //GoldenTeeMarquee::Resume();
        }

        //if we have something in the top buffer.
        if(BtmBufferLength > 0)
        {
            
            if(top_y_offset < 0)
                top_y_offset++;
            
            writeBuffer->CopyStringToFrameBuffer( top_msg, top_x_offset, TOP_ROW_Y + top_y_offset, 1, 0); 
            
            if(BtmBufferLength > 0)
            {
                
                btm_offset--;
            
                writeBuffer->CopyStringToFrameBuffer( btm_msg, btm_offset, BTM_ROW_Y, 1, 0);
                
                if(btm_offset < -btm_pixel_width)
                {
                    readyForNext = true;
                }
                 //writeBuffer->CopyStringToFrameBuffer( btm_msg, 0, BTM_ROW_Y);
            }
        }
        else
        {
            // nothing in the bottom buffer, but do we have top?
            if(TopBufferLength > 0)
            {
                //gonna do a one liner
                
                top_x_offset--;
            
                writeBuffer->CopyStringToFrameBuffer( top_msg, top_x_offset, top_y_offset, 2, 0);
                
                if(top_x_offset < -top_pixel_width)
                {
                    readyForNext = true;
                }
                 //writeBuffer->CopyStringToFrameBuffer( btm_msg, 0, BTM_ROW_Y);
            }
            else
            {
                readyForNext = true;
            }
        }
        
        //deviceSettings.ledPower++;
        //if(deviceSettings.ledPower > 60)
        //    deviceSettings.ledPower = 1;
        
        //mySign.SetBitOffIndex((byte)deviceSettings.ledPower);
        mySign->SubmitFrameBuffer(writeBuffer);
        prevTicks = currentTicks; 
    } 
}
