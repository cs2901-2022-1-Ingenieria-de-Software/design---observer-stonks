#include <iostream>
#include <unordered_map>
#include <forward_list>
#include <string>
#include <memory>
#include <vector>

using namespace std;

class ObserverInterface;
class SubjectInterface;
class WeatherStation;
class DisplayDevice;

class ObserverInterface {
public:
    ObserverInterface() = default;
    virtual ~ObserverInterface() = default;
    virtual void update(weak_ptr<SubjectInterface> s) = 0;
};

class SubjectInterface : public std::enable_shared_from_this<SubjectInterface> {
private:
    forward_list<shared_ptr<ObserverInterface>> observers;
public:
    SubjectInterface() = default;
    virtual ~SubjectInterface() = default;

    virtual void registerObserver(const shared_ptr<ObserverInterface>& obs) {
        observers.push_front(obs);
    };
    virtual void registerMultipleObserver(const vector<shared_ptr<ObserverInterface>>& obs_vec) {
        for (auto &obs : obs_vec) {
            registerObserver(obs);
        }
    };

    virtual void removeObserver(const shared_ptr<ObserverInterface>& obs) {
        observers.remove(obs);
    };

    virtual void removeMultipleObserver(const vector<shared_ptr<ObserverInterface>>& obs_vec) {
        for (const auto &obs : obs_vec) {
            removeObserver(obs);
        }
    };

    virtual void notifyObserver() {
        for (const auto &obs : observers) {
            obs->update(shared_from_this());
        }

    };

};

class WeatherStation : public SubjectInterface {

private:
    double humidity = 0;
    double temperature = 0;
    double pressure = 0;

public:

    WeatherStation() = default;
    ~WeatherStation() override = default;


    double getHumidity() const {return humidity;}

    void setHumidity(double h) {
        humidity=h;
        notifyObserver();
    };

    double getTemperature() const{return temperature;};

    void setTemperature(double t) {
        temperature=t;
        notifyObserver();
    };

    double getPressure() const{return pressure;};

    void setPressure(double p) {
        pressure=p;
        notifyObserver();
    };

};

class StatisticsObserver : public ObserverInterface{

private:
    string statistics;

public:
    StatisticsObserver() = default;
    ~StatisticsObserver() override = default;

    void update(weak_ptr<SubjectInterface> s) override {
        auto ws = dynamic_pointer_cast<WeatherStation>(shared_ptr<SubjectInterface>(s));
        statistics = "Hum:"+to_string(ws->getHumidity())+"\nTemp:"+to_string(ws->getTemperature())+"\nPres:"+to_string(ws->getPressure());

    }

    [[nodiscard]] string getStatistics() const{return statistics;}

};

class CurrentObserver : public ObserverInterface{

private:
    string current;

public:
    CurrentObserver() = default;
    ~CurrentObserver() override = default;

    void update(weak_ptr<SubjectInterface> s) override {
        auto ws = dynamic_pointer_cast<WeatherStation>(shared_ptr<SubjectInterface>(s));
        current = to_string(ws->getPressure()*0.8 + ws->getHumidity()*0.1 + ws->getTemperature()*0.1);
    }

    [[nodiscard]] string getCurrent() const{return current;}

};

class ForecastObserver : public ObserverInterface{

private:
    string forecast;

public:
    ForecastObserver() = default;
    ~ForecastObserver() override = default;

    void update(weak_ptr<SubjectInterface> s) override {
        auto ws = dynamic_pointer_cast<WeatherStation>(shared_ptr<SubjectInterface>(s));
        if (ws->getHumidity()>0.95 && ws->getPressure()>1){
            forecast="Lluvia";
        }
        else {
            forecast="Libre";
        }
    }
    [[nodiscard]] string getForecast() const{return forecast;}

};

class DisplayDevice {

private:
    shared_ptr<StatisticsObserver> statistics_obs = make_shared<StatisticsObserver>();
    shared_ptr<CurrentObserver> current_obs = make_shared<CurrentObserver>();
    shared_ptr<ForecastObserver> forecast_obs = make_shared<ForecastObserver>();

public:
    DisplayDevice() = default;
    ~DisplayDevice() = default;

    void showDisplay() const{
        cout << "STATISTICS:\n" << statistics_obs->getStatistics() << endl;
        cout << "CURRENT:\n" << current_obs->getCurrent() << endl;
        cout << "FORECAST:\n" << forecast_obs->getForecast() << endl;
    }

    [[nodiscard]] vector<shared_ptr<ObserverInterface>> getObserverList() const{
        vector<shared_ptr<ObserverInterface>> observers;
        observers.push_back( shared_ptr<ObserverInterface>(statistics_obs));
        observers.push_back( shared_ptr<ObserverInterface>(current_obs));
        observers.push_back( shared_ptr<ObserverInterface>(forecast_obs));
        return observers;
    }

};

int main() {
    auto ws = make_shared<WeatherStation>();
    auto  d1 = make_shared<DisplayDevice>();
    auto  d2 = make_shared<DisplayDevice>();
    cout << "TEST POINT 1" << endl;
    ws->registerMultipleObserver(d1->getObserverList());
    ws->registerMultipleObserver(d2->getObserverList());
    cout << "TEST POINT 2" << endl;
    ws->setHumidity(0.90);
    ws->setPressure(2);
    ws->setTemperature(10);
    cout << "TEST POINT 3" << endl;
    ws->removeMultipleObserver(d1->getObserverList()); // ACA SE ROMPE
    ws->setHumidity(0.99);
    cout << "DEVICE 1" << endl;
    d1->showDisplay();
    cout << "--------" << endl;
    cout << "DEVICE 2";
    d2->showDisplay();
    cout << "--------" << endl;
    return 0;
}

