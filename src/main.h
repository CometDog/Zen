  /*
  In this file, you map each point of the hand. A good way to do this is to draw the file in something like GIMP or 
  Photoshop. Center the newly drawn hand in a 144x168 size file and then figure out the coordinates of the points 
  by scrolling your cursor over top each point you want mapped. Then it is as simple as subtracting 72x84 from the 
  points you just mapped to get the coordinates here. You can find that easily by saving the coordinates in a .csv 
  file, opening it in Excel and using the following formula to get the points:
    =CONCATENATE("{ ", A1 - 72,", ", B1 - 84, " },")
  Then you can just copy and paste the coordinates from Excel and you will get something like what follows. It does
  not matter where you start or stop as long as you map all the points you map (don't map points twice)
*/

#pragma once

#include "pebble.h"

// Minute hand
static const GPathInfo MINUTE_HAND_POINTS = {
  // Number of points that are going to be used
  4,
  (GPoint []) {
    { 3, -5 },
    { -3, -5 },
    {-3, -55},
    { 3, -55 }
  }
};

//Tip of the minute hand
static const GPathInfo MINUTE_HAND_TIP = {
  // Number of points that are going to be used
  4,
  (GPoint []) {
    {3, -53},
    {-3, -53},
    {-3, -60},
     {3, -60}
  }
};

// Hour hand
static const GPathInfo HOUR_HAND_POINTS = {
  // Number of points that are going to be used
  4,
  (GPoint []) {
    { 3, -5 },
    { -3, -5 },
    {-3, -45},
    { 3, -45 }
  }
};

//Tip of the hour hand
static const GPathInfo HOUR_HAND_TIP = {
  // Number of points that are going to be used
  4,
  (GPoint []) {
    {3, -43},
    {-3, -43},
    {-3, -50},
     {3, -50}
  }
};

// Second hand
static const GPathInfo SECOND_HAND_POINTS = {
  // Number of points that are going to be used.
  4,
  (GPoint []) {
    { 3, -5 },
    { -3, -5 },
    {-3, -55},
    { 3, -55 }
  }
};

// Tip of the second hand
static const GPathInfo SECOND_HAND_TIP = {
  // Number of points that are going to be used.
  4,
  (GPoint []) {
    {3, -53},
    {-3, -53},
    {-3, -60},
     {3, -60}
  }
};

static const GPathInfo CENTER_POINT_OUTER = {
  // Number of points that are going to be used.
  16,
  (GPoint []) {
    { 2, 5 },
{ 4, 4 },
{ 5, 3 },
{ 6, 1 },
{ 6, -2 },
{ 5, -4 },
{ 4, -5 },
{ 2, -6 },
{ -1, -6 },
{ -3, -5 },
{ -4, -4 },
{ -5, -2 },
{ -5, 1 },
{ -4, 3 },
{ -3, 4 },
{ -1, 5 },
  }
};

static const GPathInfo CENTER_POINT_INNER = {
  // Number of points that are going to be used.
  12,
  (GPoint []) {
    { 1, 3 },
{ 2, 2 },
{ 4, 0 },
{ 4, -1 },
{ 3, -3 },
{ 1, -4 },
{ 0, -4 },
{ -2, -3 },
{ -3, -1 },
{ -3, 0 },
{ -2, 2 },
{ 0, 3 }
  }
  
};