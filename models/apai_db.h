#ifndef APAI_DB_H
#define APAI_DB_H

#include <QtCore/QString>
#include <odb/core.hxx>
#pragma db model version(1, 1)

#pragma db object
class Probes {
  public:
    Probes(const int &_id, const QString &name) : id_probe(_id), probe_name(name) {}
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
    Modes(const int &_id, const QString &name) : id_mode(_id), mode_name(name) {}
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
    PulseTypes(const int &_id, const QString &name) : id_pulse(_id), pulse_name(name) {}
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
    Apodizations(const int &_id, const QString &name) : id_apodization(_id), apodization_name(name) {}
    const QString &pulse() const { return apodization_name; }

  private:
    friend class odb::access;
    Apodizations() {}

#pragma db id
    unsigned int id_apodization;
    QString apodization_name;
};

#define printMembName(memb) qDebug << (memb, #memb);

#pragma db object
class Versions {
  public:
    Versions(const QString &name) : comments(name) {}
    const QString &pulse() const { return comments; }

  private:
    friend class odb::access;
    Versions() {}

#pragma db id auto
    unsigned int id_version;
    QString comments;
};

/*-------------------------------------*/
#pragma db value(bool) default(false)
#pragma db value(int) default(0)
#pragma db value(double) default(0) not_null
// in ODB SQLite Type Mapping, only float and double default is null others are not null. so change value program to
// not_null and make default=0;
#pragma db object no_id
struct UTPs {
  public:
    UTPs(bool manipulated) : manipulated(manipulated) {}
#pragma db index("utp_id_utp_idx") unique method("BTREE") member(id_utp, "DESC")
#pragma db index("utp_pkey_idx") unique method("BTREE")                                                                \
    members(location, nb_element, id_probe, id_apodization, freq, nb_half_cycle, polarity, transmit_lines, delta,      \
            composed_mode, duty_cycle, nb_element_max, voltage, id_pulse_type)
#pragma db index("utp_id_mtp_idx") method("BTREE") member(id_mtp, "DESC")
    int id_utp;
    int id_pulse_type;
#pragma db default(-1)
    int id_probe;
#pragma db null
    int id_mode;
    int id_apodization;
#pragma db default(-1)
    int id_mtp;
    int location;
    int nb_element;
#pragma db not_null
    double freq;
    int nb_half_cycle;
    int polarity;
    int transmit_lines;
    int composed_mode;
    int duty_cycle;
    int nb_element_max;
#pragma db not_null
    double voltage;
    int delta;
#pragma db not_null
    double deltaT;
    bool manipulated;
};

#pragma db object
struct UTPInfos {
#pragma db id
    int id_utp;
#pragma db not_null
    double framerate_multiplier;
    double framerate_multiplier_m;
    double max_voltage;
    double voltage;
    double mi;
    double tib;
    double tis;
    double ispta;
    int valid;
    QString presets;
    double tic;
    QString settings;
};

#pragma db not_null
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
    double T; // steven add this for temperature --eton@22/02/21
};
#endif // APAI_DB_H
