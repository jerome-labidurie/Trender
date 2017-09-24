/**
 *  This file is part of Trender.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2016 Jérôme Labidurie jerome@labidurie.fr
 *  Copyright 2016 Cédric Bernard cedric.bernard@galilabs.com
 *  Copyright 2016 Julien Jacques julien.jacques@galilabs.com
 */
 
#include "inc/TDR_Jenkins.h"

// class TDR_Jenkins : public TDR_Usage 
TDR_Jenkins::TDR_Jenkins(){
	
    _pstrip    = NULL;

}
TDR_Jenkins::TDR_Jenkins(TDR_NeoPixel* n, TDR_ThingSpeak* t) {
    
    _pstrip    = n;
    _pTS       = t;
}

TDR_Jenkins::~TDR_Jenkins(){
	
}

void TDR_Jenkins::setStrip(TDR_NeoPixel* s) {
    _pstrip=s;
}
void TDR_Jenkins::setThingSpeak(TDR_ThingSpeak* t) {
    _pTS=t;
}

TDR_NeoPixel* TDR_Jenkins::getStrip() {
	return _pstrip;
}
TDR_ThingSpeak* TDR_Jenkins::getThingSpeak() {
    return _pTS;
}

char* TDR_Jenkins::get_type() {
    return _type;
}

unsigned char TDR_Jenkins::get_last() {
    return _pTS->get_last_field1();
}

uint8_t  TDR_Jenkins::setStripColor(uint8_t R, uint8_t G, uint8_t B) {
    uint32_t c = (R<<16) && (G<<8) && B;
    _pstrip->gotoColor(c,300);

    return TDR_SUCCESS;
}



// end of file