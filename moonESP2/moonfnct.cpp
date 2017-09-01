#include "moonfnct.h"

/*
 * JYEAR  --  Convert Julian date to year, month, day, which are
 *         returned via integer pointers to integers.  
 */
void jyear(double td, int *yy, int *mm, int *dd)
{
    double j, d, y, m;

    td += 0.5;              /* Astronomical to civil */
    j = floor(td);
    j = j - 1721119.0;
    y = floor(((4 * j) - 1) / 146097.0);
    j = (j * 4.0) - (1.0 + (146097.0 * y));
    d = floor(j / 4.0);
    j = floor(((4.0 * d) + 3.0) / 1461.0);
    d = ((4.0 * d) + 3.0) - (1461.0 * j);
    d = floor((d + 4.0) / 4.0);
    m = floor(((5.0 * d) - 3) / 153.0);
    d = (5.0 * d) - (3.0 + (153.0 * m));
    d = floor((d + 5.0) / 5.0);
    y = (100.0 * y) + j;
    if (m < 10.0)
        m = m + 3;
    else {
        m = m - 9;
        y = y + 1;
    }
    *yy = y;
    *mm = m;
    *dd = d;
}

/*
 * JHMS  --  Convert Julian time to hour, minutes, and seconds.
 */
void jhms(double j, int *h, int *m, int *s)
{
    long ij;

    j += 0.5;               /* Astronomical to civil */
    ij = (j - floor(j)) * 86400.0;
    *h = ij / 3600L;
    *m = (ij / 60L) % 60L;
    *s = ij % 60L;
}


/*
 * MEANPHASE  --  Calculates mean phase of the Moon for a given
 *      base date.  This argument K to this function is
 *      the precomputed synodic month index, given by:
 *
 *          K = (year - 1900) * 12.3685
 *
 *      where year is expressed as a year and fractional
 *      year.
 */
double meanphase(double sdate, double k)
{
    double  t, t2, t3, nt1;

    /* Time in Julian centuries from 1900 January 0.5 */
    t = (sdate - 2415020.0) / 36525;
    t2 = t * t;        /* Square for frequent use */
    t3 = t2 * t;           /* Cube for frequent use */

    nt1 = 2415020.75933 + synmonth * k
        + 0.0001178 * t2
        - 0.000000155 * t3
        + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

    return nt1;
}

/*
 * TRUEPHASE  --  Given a K value used to determine the
 *      mean phase of the new moon, and a phase
 *      selector (0.0, 0.25, 0.5, 0.75), obtain
 *      the true, corrected phase time.
 */
double truephase(double k, double phase)
{
    double t, t2, t3, pt, m, mprime, f;
    int apcor = FALSE;

    k += phase;        /* Add phase to new moon time */
    t = k / 1236.85;       /* Time in Julian centuries from
                      1900 January 0.5 */
    t2 = t * t;        /* Square for frequent use */
    t3 = t2 * t;           /* Cube for frequent use */
    pt = 2415020.75933     /* Mean time of phase */
         + synmonth * k
         + 0.0001178 * t2
         - 0.000000155 * t3
         + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

        m = 359.2242               /* Sun's mean anomaly */
        + 29.10535608 * k
        - 0.0000333 * t2
        - 0.00000347 * t3;
        mprime = 306.0253          /* Moon's mean anomaly */
        + 385.81691806 * k
        + 0.0107306 * t2
        + 0.00001236 * t3;
        f = 21.2964                /* Moon's argument of latitude */
        + 390.67050646 * k
        - 0.0016528 * t2
        - 0.00000239 * t3;
    if ((phase < 0.01) || (abs(phase - 0.5) < 0.01)) {

       /* Corrections for New and Full Moon */
       pt +=     (0.1734 - 0.000393 * t) * dsin(m)
            + 0.0021 * dsin(2 * m)
            - 0.4068 * dsin(mprime)
            + 0.0161 * dsin(2 * mprime)
            - 0.0004 * dsin(3 * mprime)
            + 0.0104 * dsin(2 * f)
            - 0.0051 * dsin(m + mprime)
            - 0.0074 * dsin(m - mprime)
            + 0.0004 * dsin(2 * f + m)
            - 0.0004 * dsin(2 * f - m)
            - 0.0006 * dsin(2 * f + mprime)
            + 0.0010 * dsin(2 * f - mprime)
            + 0.0005 * dsin(m + 2 * mprime);
       apcor = TRUE;
    } else if ((abs(phase - 0.25) < 0.01 || (abs(phase - 0.75) < 0.01))) {
       pt +=     (0.1721 - 0.0004 * t) * dsin(m)
            + 0.0021 * dsin(2 * m)
            - 0.6280 * dsin(mprime)
            + 0.0089 * dsin(2 * mprime)
            - 0.0004 * dsin(3 * mprime)
            + 0.0079 * dsin(2 * f)
            - 0.0119 * dsin(m + mprime)
            - 0.0047 * dsin(m - mprime)
            + 0.0003 * dsin(2 * f + m)
            - 0.0004 * dsin(2 * f - m)
            - 0.0006 * dsin(2 * f + mprime)
            + 0.0021 * dsin(2 * f - mprime)
            + 0.0003 * dsin(m + 2 * mprime)
            + 0.0004 * dsin(m - 2 * mprime)
            - 0.0003 * dsin(2 * m + mprime);
       if (phase < 0.5)
          /* First quarter correction */
          pt += 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
       else
          /* Last quarter correction */
          pt += -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);
       apcor = TRUE;
    }
    if (!apcor) {
       abort();
    }
    return pt;
}

/*
 * PHASEHUNT  --  Find time of phases of the moon which surround
 *      the current date.  Five phases are found, starting
 *      and ending with the new moons which bound the
 *      current lunation.
 */
void phasehunt(double sdate, double phases[5])
{
    int yy, mm, dd;
    double  adate, k1, k2, nt1, nt2;

    adate = sdate - 45;
    jyear(adate, &yy, &mm, &dd);
    k1 = floor((yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

    adate = nt1 = meanphase(adate, k1);
    while(1) {
        adate += synmonth;
        k2 = k1 + 1;
        nt2 = meanphase(adate, k2);
        if (nt1 <= sdate && nt2 > sdate)
            break;
        nt1 = nt2;
        k1 = k2;
    }
    phases[0] = truephase(k1, 0.0);
    phases[1] = truephase(k1, 0.25);
    phases[2] = truephase(k1, 0.5);
    phases[3] = truephase(k1, 0.75);
    phases[4] = truephase(k2, 0.0);
}

double nextmoon(double sdate, double phase)
{
    int yy, mm, dd;
    double k, ret;

    jyear(sdate, &yy, &mm, &dd);
    k = floor((yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

    while((ret=truephase(k, phase)) < sdate) {
        k++;
    }

    return ret;
}


/*
 * KEPLER  --   Solve the equation of Kepler.
 */
double kepler(double m, double ecc)
{
    double e, delta;

    e = m = torad(m);
    do {
        delta = e - ecc * sin(e) - m;
        e -= delta / (1 - ecc * cos(e));
    } while (abs (delta) > EPSILON);
    return e;
}

/*
 * JDATE  --  Convert internal GMT date and time to Julian day
 *         and fraction.
 */
double jdate(tmElements_t *t)
{
    long        c, m, y;

    y = t->Year + 1970;
    m = t->Month;
    if (m > 2) {
        m = m - 3;
    } else {
        m = m + 9;
        y--;
    }
    c = y / 100L;          /* Compute century */
    y -= 100L * c;
    return (t->Day + (c * 146097L) / 4 + (y * 1461L) / 4 +
        (m * 153L + 2) / 5 + 1721119L);
}

/*
 * JTIME --    Convert internal GMT date and time to astronomical Julian
 *         time (i.e. Julian date plus day fraction, expressed as
 *         a double).
 */
double jtime(tmElements_t *t)
{
    return (jdate (t) - 0.5) +
       (t->Second + 60 * (t->Minute + 60 * t->Hour)) / 86400.0;
}

/*
 * PHASE  --  Calculate phase of moon as a fraction:
 *
 *  The argument is the time for which the phase is requested,
 *  expressed as a Julian date and fraction.  Returns the terminator
 *  phase angle as a percentage of a full circle (i.e., 0 to 1),
 *  and stores into pointer arguments the illuminated fraction of
 *  the Moon's disc, the Moon's age in days and fraction, the
 *  distance of the Moon from the centre of the Earth 
 */
double phase(
        double pdate,
        double *pphase,     /* Illuminated fraction */
        double *mage,       /* Age of moon in days */
        double *dist,       /* Distance in kilometres */
        double *sudist      /* Distance to Sun */
        )
{
    double  Day, N, M, Ec, Lambdasun, ml, MM, Ev, Ae, A3, MmP,
        mEc, A4, lP, V, lPP, MoonAge, MoonPhase, MoonDist, F, SunDist;

    /******** Calculation of the Sun's position ********/
    /* Date within epoch */
    Day = pdate - epoch;
    /* Mean anomaly of the Sun */
    N = fixangle((360 / 365.2422) * Day);
    /* Convert from perigee co-ordinates to epoch 1980.0 */
    M = fixangle(N + elonge - elongp);
    /* Solve equation of Kepler */
    Ec = kepler(M, eccent);
    Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
    Ec = 2 * todeg(atan(Ec));       /* True anomaly */
    /* Sun's geocentric ecliptic longitude */
    Lambdasun = fixangle(Ec + elongp);
    /* Orbital distance factor */
    F = ((1 + eccent * cos(torad(Ec))) / (1 - eccent * eccent));
    /* Distance to Sun in km */
    SunDist = sunsmax / F;

    /******** Calculation of the Moon's position ********/
    /* Moon's mean longitude */
    ml = fixangle(13.1763966 * Day + mmlong);
    /* Moon's mean anomaly */
    MM = fixangle(ml - 0.1114041 * Day - mmlongp);
    /* Evection */
    Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM));
    /* Annual equation */
    Ae = 0.1858 * sin(torad(M));
    /* Correction term */
    A3 = 0.37 * sin(torad(M));
    /* Corrected anomaly */
    MmP = MM + Ev - Ae - A3;
    /* Correction for the equation of the centre */
    mEc = 6.2886 * sin(torad(MmP));
    /* Another correction term */
    A4 = 0.214 * sin(torad(2 * MmP));
    /* Corrected longitude */
    lP = ml + Ev + mEc - Ae + A4;
    /* Variation */
    V = 0.6583 * sin(torad(2 * (lP - Lambdasun)));
    /* True longitude */
    lPP = lP + V;

    /******** Calculation of the phase of the Moon ********/
    /* Age of the Moon in degrees */
    MoonAge = lPP - Lambdasun;
    /* Phase of the Moon */
    MoonPhase = (1 - cos(torad(MoonAge))) / 2;
    /* Calculate distance of moon from the centre of the Earth */
    MoonDist = (msmax * (1 - mecc * mecc)) / (1 + mecc * cos(torad(MmP + mEc)));

    *pphase = MoonPhase;
    *mage = synmonth * (fixangle(MoonAge) / 360.0);
    *dist = MoonDist;
    *sudist = SunDist;
    return fixangle(MoonAge) / 360.0;
}

