#pragma once
#include <string>
#include "shakki.h"

class Siirto
{
public:
    
    Siirto() : _a_r(0), _a_l(0), _l_r(0), _l_l(0) {}

    Siirto(int a_r, int a_l, int l_r, int l_l) :
        _a_r(a_r), _a_l(a_l), _l_r(l_r), _l_l(l_l)
    {}

    // Konstruktori, joka muuntaa merkkijonon siirroksi, esim. "e2e4" tai "g8f6".
    Siirto(const std::string& s)
    {
        _a_l = s[0] - 'a';
        _a_r = '8' - s[1];
        _l_l = s[2] - 'a';
        _l_r = '8' - s[3];

        if (s.size() > 4)
        {
            if (_a_r == 0) {

                switch (s[4])
                {
                case 'Q':
                    _korotettava_nappula = wQ;
                    break;
                case 'R':
                    _korotettava_nappula = wR;
                    break;
                case 'B':
                    _korotettava_nappula = wB;
                    break;
                case 'N':
                    _korotettava_nappula = wN;
                    break;
                }
            }
            else {
                switch (s[4])
                {
                case 'Q':
                    _korotettava_nappula = bQ;
                    break;
                case 'R':
                    _korotettava_nappula = bR;
                    break;
                case 'B':
                    _korotettava_nappula = bB;
                    break;
                case 'N':
                    _korotettava_nappula = bN;
                    break;
                }
            }
        }
    }

    // Vertailuoperaattori: ovatko kaksi siirto-oliota identtisiä?
    bool operator==(const Siirto& rhs) const
    {
        return _a_r == rhs._a_r && _a_l == rhs._a_l && _l_r == rhs._l_r && _l_l == rhs._l_l;
    }

    // Palauttaa siirron muotoillun merkkijonon
    std::string muotoiltu_merkkijono() const
    {
        std::string siirto_string;
        siirto_string += char('a' + _a_l);
        siirto_string += char('8' - _a_r);
        siirto_string += char('a' + _l_l);
        siirto_string += char('8' - _l_r);
        return siirto_string;
    }

private:
    int _a_r; // siirron alkuruudun rivi
    int _a_l; // siirron alkuruudu linja (eli sarake)

    int _l_r; // siirron loppuruudun rivi
    int _l_l; // siirron loppuruudun linja

    int _korotettava_nappula = NA; // esim. wQ, wR, bN jne.
    bool _ohestalyonti = false;

    friend class Asema;
};
