#ifndef NEWNUFLUX_LEFLUX_H
#define NEWNUFLUX_LEFLUX_H

#include <nuflux/nuflux.h>

namespace nuflux{

  class IntegralPreservingFlux : public FluxFunction{
  public:
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

    IntegralPreservingFlux(const std::string& fluxName);

    virtual ~IntegralPreservingFlux();

    bool SetSubGeV;

    virtual double getMinEnergy() const;
    virtual double getMaxEnergy() const;

    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    virtual double getFlux(ParticleType type, double energy, double azimuth, double cosZenith) const;

  private:
    ///A really simple histogram.
    ///Must be filled from left to right.
    struct dumbHistogram{
    public:
      ///Construct a histogram initialized with the lower edge of its first bin
      explicit dumbHistogram(double firstEdge){
        binEdges.push_back(firstEdge);
      }
      ///Append the upper edge of the next bin and the value contained in that bin.
      ///\pre edge > all edges previously added
      void push_back(double edge, double value){
        assert(edge>binEdges.back());
        binEdges.push_back(edge);
        values.push_back(value);
      }
      ///\return the width of bin i
      double binWidth(unsigned int i) const{
        assert(i<values.size());
        return(binEdges[i+1]-binEdges[i]);
      }
      ///\return the value in bin i
      double value(unsigned int i) const{
        assert(i<values.size());
        return(values[i]);
      }
      ///Compute the integral of the histogram
      ///\return a pair of vectors, which contain coordinates (bin edges) and
      ///        accumulated values, respectively
      std::pair<std::vector<double>,std::vector<double> > accumulate() const{
        std::pair<std::vector<double>,std::vector<double> > acc;
        double sum=0;
        acc.first.push_back(binEdges[0]);
        acc.second.push_back(sum);
        for(unsigned int i=0; i<values.size(); i++){
          sum+=values[i]*binWidth(i);
          acc.first.push_back(binEdges[i+1]);
          acc.second.push_back(sum);
        }
        return(acc);
      }
    private:
      std::vector<double> binEdges, values;
    };

    //TODO: expose CubicSpline for testing
    class CubicSpline{
    private:
      struct splineSegment{
        double x;
        double a,b,c,d;

        double operator()(double x) const{
          double xs=x-this->x;
          return(((a*xs+b)*xs+c)*xs+d);
        }

        double derivative(double x) const{
          double xs=x-this->x;
          return((3*a*xs+2*b)*xs+c);
        }
      };
      std::vector<splineSegment> data;
      double xlast; //upper bound of interpolation domain
      double mlast, blast; //coefficients for extrapolation beyond xlast

      ///\pre x is within the domain of the spline
      const splineSegment& findSegment(double x) const{
        unsigned int iMin=0, iMax=data.size()-1;
        while(iMin<iMax){
          unsigned int idx=(iMin+iMax)/2;
          if(data[idx].x>x)
            iMax=idx-1;
          else if(data[idx+1].x<=x)
            iMin=idx+1;
          else{
            iMin=idx;
            break;
          }
        }
        return(data[iMin]);
      }
    public:
      CubicSpline(){}

      CubicSpline(const std::vector<double>& x, const std::vector<double>& y):
        data(x.size()-1){
        const unsigned int N=x.size();
        assert(y.size()==N);

        //store h values in a coefficients
        //store beta values in c coefficients
        std::vector<double> M(N);
        data[0].a=x[1]-x[0];
        data[1].a=x[2]-x[1];
        M[0]=6.*((y[2]-y[1])/data[1].a+(y[0]-y[1])/data[0].a);
        data[1].c=2*(x[2]-x[0]);
        for(unsigned int i=1; i<N-2; i++){
          data[i+1].a=x[i+2]-x[i+1];
          data[i+1].c=2.*(x[i+2]-x[i])-data[i].a*data[i].a/data[i].c;
          M[i]=6.*((y[i+2]-y[i+1])/data[i+1].a+(y[i]-y[i+1])/data[i].a)-data[i].a*M[i-1]/data[i].c;
        }
        M[N-2]=M[N-3]/data[N-2].c;
        data[N-2].x=x[N-2];
        data[N-2].c=(y[N-1]-y[N-2])/data[N-2].a-2.*M[N-2]*data[N-2].a/6.;
        data[N-2].a=(-M[N-2])/(6.*data[N-2].a);
        data[N-2].b=M[N-2]/2.;
        data[N-2].d=y[N-2];
        for(unsigned int i=N-3; i>0; i--){
          M[i]=(M[i-1]-(x[i+2]-x[i+1])*M[i+1])/data[i].c;
          data[i].x=x[i];
          data[i].c=(y[i+1]-y[i])/data[i].a-(M[i+1]+2.*M[i])*data[i].a/6.;
          data[i].a=(M[i+1]-M[i])/(6.*data[i].a);
          data[i].b=M[i]/2.;
          data[i].d=y[i];
        }
        data[0].x=x[0];
        data[0].c=(y[1]-y[0])/data[0].a-M[1]*data[0].a/6.;
        data[0].a=M[1]/(6.*data[0].a);
        data[0].b=0.0;
        data[0].d=y[0];

        xlast=x[N-1];
        double xslast = x[N-1]-x[N-2];
        mlast = (3*data.back().a*xslast + 2*data.back().b)*xslast + data.back().c;
        blast = ((data[N-2].a*xslast+data[N-2].b)*xslast+data[N-2].c)*xslast+data[N-2].d;
      }

      double operator()(double x){
        if(data.empty()){
          throw std::runtime_error("Attempting to evaluate empty spline");
        }
        if(x<data.front().x){
          return(data.front().c*(x-data.front().x)+data.front().d);
        }
        if(x>xlast){
          return(mlast*(x-xlast)+blast);
        }
        return(findSegment(x)(x));
      }

      double derivative(double x) const{
        if(data.empty()){
          throw std::runtime_error("Attempting to evaluate empty spline");
        }
        if(x<data.front().x){
          return(data.front().c);
        }
        if(x>xlast){
          return(mlast);
        }
        return(findSegment(x).derivative(x));
      }
    };

    void loadTables(const std::string& fluxName, ParticleType type);

    double evaluate2D(ParticleType type, double energy, double cosZenith) const;
    double InterpolateAzimuth(ParticleType type, double energy, double azimuth, double cosZenith) const;
    double InterpolateCZFlux(ParticleType type, double energy, double azimuth, double cosZenith) const;

    std::map<ParticleType, std::map<double, CubicSpline> > energySplines2D;
    std::map<ParticleType, std::map<std::pair<double,double>,CubicSpline> > energySplines3D;
  };

} //namespace nuflux

#endif //NEWNUFLUX_LEFLUX_H
