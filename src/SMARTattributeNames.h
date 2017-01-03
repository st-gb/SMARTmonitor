/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTattributeNames.h
 * Author: sg
 *
 * Created on 2. Januar 2017, 23:36
 */

#ifndef SMARTATTRIBUTENAMES_H
#define SMARTATTRIBUTENAMES_H

#ifdef __cplusplus
namespace SMARTattributeNames
{
extern "C" {
#endif

/** See https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes */
enum SMARTattributeNames 
{
    /** https://de.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#.C3.9Cbliche_Parameter : 
    Laufleistung in Stunden oder Sekunden (inklusive Standby)
    Deutet auf Abnutzung hin, sagt aber nichts über Nutzungsumstände in dieser Zeit aus.
    Bei einigen Modellen von Maxtor, z. B. bei der Maxtor DiamondMax 10 6L250S0 
    * sind das Minuten. */
  PowerOnTime = 9,
  TemperatureInMilliKelvin = 194
};

#ifdef __cplusplus
} //namespace SMARTattributeNames
}
#endif

#endif /* SMARTATTRIBUTENAMES_H */

