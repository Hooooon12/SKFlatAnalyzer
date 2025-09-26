#include "Particle.h"

ClassImp(Particle)

Particle::Particle() :
  TLorentzVector(),
  j_Charge(0)
{
  k_debug=false;
}

Particle::Particle(const TLorentzVector& p) :
  TLorentzVector(p),
  j_Charge(0)
{
}

Particle::Particle(const Particle& p) :
  TLorentzVector(p),
  j_Charge(p.Charge())
{
}

Particle::Particle(double px, double py, double pz, double e) :
  TLorentzVector(px, py, pz, e),
  j_Charge(0)
{
}


Particle& Particle::operator=(const Particle& p){
  if(this != &p){
    TLorentzVector::operator=(p);
    j_Charge = p.j_Charge;
  }

  return *this;
}

Particle& Particle::operator+= (const Particle& p){
  TLorentzVector::operator+=(p);
  j_Charge += p.j_Charge;
  return *this;
}


Particle::~Particle()
{
}

void Particle::SetCharge(double q){
  j_Charge = q;
}

std::ostream& operator<<(std::ostream& os, const Particle& p) {
  os << "(Pt, Eta, Phi, M, Charge) = "
     << p.Pt()  << "\t"
     << p.Eta() << "\t"
     << p.Phi() << "\t"
     << p.M()   << "\t"
     << p.Charge();
  return os;
}

void Particle::Print() const {
  std::cout << *this << std::endl;
}

