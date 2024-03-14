#include <iostream>
#include <string>
#include <vector>
#include "asema.h" 
#include "siirto.h"

using namespace std;

int main()
{
    // Alkuasema.
    Asema asema;

    while (true)
    {
        // Tulostetaan nykyinen asema
        asema.tulosta();

        // Haetaan kaikki mahdolliset siirrot
        vector<Siirto> siirrot;
        asema.anna_siirrot(siirrot);

        cout << "Mahdolliset siirrot:" << endl;
        int siirrot_laskuri = 0;

        // Laittaa lailliset siirrot siististi riveihin
        for (const auto& siirto : siirrot)
        {
            cout << siirto.muotoiltu_merkkijono() << " ";
            siirrot_laskuri++;
            if (siirrot_laskuri % 5 == 0) {
                cout << endl;
            }
        }
        if (siirrot_laskuri % 5 != 0) {
            cout << endl;
        }

        // Tulostetaan laillisten siirtojen lukumäärä
        cout << "Laillisten siirtojen lukumäärä: " << siirrot.size() << endl;

        if (siirrot.empty()) {
            // Tarkista lopputulos, jos pelaajalla ei ole laillisia siirtoja jäljellä
            float lopputulos = asema.pisteyta_lopputulos();
            if (lopputulos == 0) {
                cout << "Peli päättyi pattiin." << endl;
                break;
            }
            else if (lopputulos <= -1000000) {
                cout << "Valkoinen kuningas on matissa hävisit pelin..." << endl;
                break;
            }
        }

        // Kysytään pelaajan siirto
        string pelaajan_siirto;
        cout << "Syötä siirto: ";
        cin >> pelaajan_siirto;

        // Luodaan Siirto-olio pelaajan syötteen perusteella
        Siirto pelaajan_siirto_obj(pelaajan_siirto);

        // Tarkistetaan onko pelaajan syöttämä siirto mahdollinen
        bool on_laillinen_siirto = false;
        for (const auto& siirto : siirrot)
        {
            if (siirto == pelaajan_siirto_obj)
            {
                on_laillinen_siirto = true;
                break;
            }
        }

        // Jos siirto on laillinen tehdään se ja tulostetaan asema
        if (on_laillinen_siirto)
        {
            // Tee pelaajan siirto laudalla
            asema.tee_siirto(pelaajan_siirto_obj);
            asema.tulosta();
        }
        else
        {
            cout << "Virheellinen siirto! Yritä uudelleen." << endl;
            continue; // Palaa kysymään uutta siirtoa
        }

        // Tietokone tekee siirron
        MinimaxArvo tietokoneen_siirto = asema.minimax(1);
        asema.tee_siirto(tietokoneen_siirto._siirto);

        // Tarkista lailliset siirrot
        vector<Siirto> lailliset_siirrot;
        asema.anna_siirrot(lailliset_siirrot);

        // Jos ei ole laillisia siirtoja tarkista lopputulos
        if (lailliset_siirrot.empty()) {
            // Tarkista lopputulos
            float lopputulos = asema.pisteyta_lopputulos();
            if (lopputulos >= 1000000) {
                cout << "Musta Kuningas on matissa, voitit pelin." << endl;
                break;
            }
            else if (lopputulos == 0) {
                cout << "Peli päättyi pattiin." << endl;
                break;
            }
        }

        cout << "Tietokoneen siirto: " << tietokoneen_siirto._siirto.muotoiltu_merkkijono() << endl;
    }

    return 0;
}
