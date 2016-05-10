package com.manav.app2;

/**
 * Created by manav on 5/5/16.
 */
public class DataHandler {
    String temp,lpg,humidity;
    int fire,intrusion;

    public String getTemp() {
        return temp;
    }

    public void setTemp(String temp) {
        this.temp = temp;
    }

    public String getLpg() {
        return lpg;
    }

    public void setLpg(String lpg) {
        this.lpg = lpg;
    }

    public String getHumidity() {
        return humidity;
    }

    public void setHumidity(String humidity) {
        this.humidity = humidity;
    }

    public int getFire() {
        return fire;
    }

    public void setFire(int fire) {
        this.fire = fire;
    }

    public int getIntrusion() {
        return intrusion;
    }

    public void setIntrusion(int intrusion) {
        this.intrusion = intrusion;
    }
}
