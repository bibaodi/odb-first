#ifndef APAI_DB_H
#define APAI_DB_H

#include <QtCore/QString>
#include <odb/core.hxx>
#pragma db model version(1, 1)

#pragma db object
class Probes {
  public:
    Probes(const QString &name) : probe_name(name) {}
    const QString &probe() const { return probe_name; }

  private:
    friend class odb::access;
    Probes() {}

#pragma db id
    unsigned int id_probe;
    QString probe_name;
};

#pragma db object
class Modes {
  public:
    Modes(const QString &name) : mode_name(name) {}
    const QString &mode() const { return mode_name; }

  private:
    friend class odb::access;
    Modes() {}

#pragma db id
    unsigned int id_mode;
    QString mode_name;
};

#pragma db object
class PulseTypes {
  public:
    PulseTypes(const QString &name) : pulse_name(name) {}
    const QString &pulse() const { return pulse_name; }

  private:
    friend class odb::access;
    PulseTypes() {}

#pragma db id
    unsigned int id_pulse;
    QString pulse_name;
};

#pragma db object
class Apodizations {
  public:
    Apodizations(const QString &name) : apodization_name(name) {}
    const QString &pulse() const { return apodization_name; }

  private:
    friend class odb::access;
    Apodizations() {}

#pragma db id
    unsigned int id_apodization;
    QString apodization_name;
};

#pragma db object
class Versions {
  public:
    Versions(const QString &name) : comments(name) {}
    const QString &pulse() const { return comments; }

  private:
    friend class odb::access;
    Versions() {}

#pragma db id
    unsigned int id_version;
    QString comments;
};

/*-------------------------------------*/
#pragma db object
struct UTPs {
#pragma db id
    int id_utp;
    int id_pulse_type;
    int id_probe;
    int id_mode;
    int id_apodization;
    int id_mtp;
    int location;
    int nb_element;
    double freq;
    int nb_half_cycle;
    int polarity;
    int transmit_lines;
    int composed_mode;
    int duty_cycle;
    int nb_element_max;
    double voltage;
    int delta;
    int deltaT;
    bool manipulated;
};

#pragma db object
struct UTPInfos {
#pragma db id
    int id_utp;
    double framerate_multiplier;
    double framerate_multiplier_m;
    double max_voltage;
    double voltage;
    double mi;
    double tib;
    double tis;
    double ispta;
    int valid;
    QString preset;
    double tic;
    QString settings;
};

#pragma db object
struct MTPs {
#pragma db id
    int id_mtp;
    int id_utp;
    int T;
    double mi;
    double mi_inv;
    double pii_0_u;
    double pii_3_u;
    double pii_0_s;
    double pii_3_s;
    double pii_3_inv_u;
    double pii_3_inv_s;
    double w0;
    double tisas_u;
    double tisas_s_factor;
    double tisbs_u;
    double tibbs_u;
    double ticas_u;
    double ticas_s_factor;
    double td_u;
    double td_s;
    double ec_hv;
    double pd;
    double pr;
    double pc;
    double isppa;
    double ispta;
    double fc3;
    double fc6;
    double pr3;
    double tisbs_s;
    double tibbs_s;
    double w0_s;
    double z0;
    double z3;
    double sigma;
    double delta;
    double ispta_s;
    double ipa3_mi;
    double ispta3_s;
    double FL_Azim;
    double FL_Elev;
    double AaptAzim;
    double deq4MI;
    double deq4TIB;
    double zB3;
    double w01_mW;
    double zBP_cm;
    double z1_cm;
    double minW3ITA3_mW;
    double ticas_s;
    double tisas_s;
};
#endif // APAI_DB_H
