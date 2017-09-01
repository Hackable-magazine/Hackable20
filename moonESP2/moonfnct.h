#include <time.h>
// soft i2c par défaut nodeMCU :
//   SDA = 4 = D2
//   SCL = 5 = D1
#include <DS3232RTC.h>
//#include <DS1307RTC.h>

#define TRUE  1
#define FALSE 0

/*  Astronomical constants  */

#define epoch       2444238.5      /* 1980 January 0.0 */

/*  Constants defining the Sun's apparent orbit  */

#define elonge      278.833540     /* Ecliptic longitude of the Sun at epoch 1980.0 */
#define elongp      282.596403     /* Ecliptic longitude of the Sun at perigee */
#define eccent      0.016718       /* Eccentricity of Earth's orbit */
#define sunsmax     1.495985e8     /* Semi-major axis of Earth's orbit, km */

/*  Elements of the Moon's orbit, epoch 1980.0  */

#define mmlong      64.975464      /* Moon's mean lonigitude at the epoch */
#define mmlongp     349.383063     /* Mean longitude of the perigee at the epoch */
#define mecc        0.054900       /* Eccentricity of the Moon's orbit */
#define msmax       384401.0       /* Semi-major axis of Moon's orbit in km */
#define synmonth    29.53058868    /* Synodic month (new Moon to new Moon) */

#define PI 3.14159265358979323846  /* Assume not near black hole nor in Tennessee */
#define EPSILON 1E-6

/*  Handy mathematical functions  */

#define abs(x) ((x) < 0 ? (-(x)) : (x))           /* Absolute val */
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle    */
#define torad(d) ((d) * (PI / 180.0))             /* Deg->Rad     */
#define todeg(d) ((d) * (180.0 / PI))             /* Rad->Deg     */
#define dsin(x) (sin(torad((x))))             /* Sin from deg */
#define dcos(x) (cos(torad((x))))             /* Cos from deg */

// convertir une date en jours julien en années, mois et jour
void jyear(double td, int *yy, int *mm, int *dd);
// convertir une date en jours julien en heures, minutes et secondes
void jhms(double j, int *h, int *m, int *s);
// retourne la date de la nouvelle lune
double meanphase(double sdate, double k);
// retourne la date précise pour une phase lunaire donnée
double truephase(double k, double phase);
// donne des information sur les prochaines phases lunaires
void phasehunt(double sdate, double phases[5]);
// retourne la date de la prochaine phase spécifiée
double nextmoon(double sdate, double phase);
// calcule l'équation de Kepler
double kepler(double m, double ecc);
// converti une date standard en jours julien
double jdate(tmElements_t *t);
// fait de même mais avec l'heure en plus
double jtime(tmElements_t *t);
// calcul divers données lunaires
double phase(double pdate, double *pphase, double *mage, double *dist, double *sudist);

