/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   columnIndices.hpp
 * Author: sg
 * Created on 11. März 2017, 17:39 */

#ifndef COLUMNINDICES_HPP
#define COLUMNINDICES_HPP

/** Also a service might show something. So place this file under folder
 *  "UserInterface" or "SMARTmonitorBase" */
namespace ColumnIndices
{
  //TODO maybe move to a distinct/seperate header file because the whole class
  //declaration of "SMARTmonitorClient" is not needed by 
  //wxWidgets::SMARTtableListCtrl
  enum columnIndices { SMART_ID = 0 , SMARTparameterName,
    rawValue, humanReadableRawValue, lastUpdate, beyondLast };
}

#endif /* COLUMNINDICES_HPP */

