#pragma once
#include <limits>
#include <map>
#include <vector>
#include "shakki.h"
#include "siirto.h"
using namespace std;


// Minimax-funktion palauttama arvo. Sisältää sekä
// minimax-arvon että sitä vastaavan parhaan siirron.
class MinimaxArvo
{
public:
	MinimaxArvo(float arvo, Siirto siirto) :
		_arvo(arvo), _siirto(siirto)
	{}

	float	_arvo;
	Siirto	_siirto;
};


class Asema
{
public:

	// Tyhjentää laudan.
	void tyhjenna();

	// Tekee annetun siirron laudalla. Voidaan olettaa, että
	// siirto on laillinen.
	void tee_siirto(const Siirto& s);

	// Tulostaa lauta ascii-grafiikkana.
	void tulosta() const;

	// SIIRTOGENERAATTORI
	// Tuottaa aseman kaikki LAILLISET siirrot.
	void anna_siirrot(std::vector<Siirto>& siirrot) const
	{
		int kuningas = _siirtovuoro == VALKEA ? wK : bK;
		int pelaaja = _siirtovuoro;
		int vihu = vastustaja(pelaaja);

		// Luodaan pelaajan raakasiirrot.
		std::vector<Siirto> raakasiirrot;
		anna_kaikki_raakasiirrot(pelaaja, raakasiirrot);
		anna_linnoitukset(pelaaja, raakasiirrot);

		// Testataan jokainen raakasiirto.
		for (Siirto& rs : raakasiirrot)
		{
			// Luodaan kopio nykyisestä asemasta.
			Asema testiasema = *this;

			// Pelataan raakasiirto testiasemassa.
			testiasema.tee_siirto(rs);

			// Etsitään kuningas.
			int rivi, sarake;
			testiasema.etsi_nappula(kuningas, rivi, sarake);

			// Tarkistetaan jääkö kuningas uhatuksi testiaseman jälkeen.
			if (!testiasema.onko_ruutu_uhattu(rivi, sarake, vihu))
			{
				// Tarkistetaan myös ettei siirto jätä kuninkaan uhatuksi.
				if (!testiasema.onko_ruutu_uhattu(rivi, sarake, pelaaja))
				{
					// Siirto ei jätä kuningasta uhatuksi.
					siirrot.push_back(rs);
				}
			}
		}
	}


	// Pisteyttää pelin lopputuloksen seuraavasti:
	//
	// Valkea tehnyt matin			1000000
	// Tasapeli (patti)				0
	// Musta tehnyt matin		   -1000000
	//
	// Funktiota kutsutaan, kun asemassa ei ole enää yhtään laillista
	// siirtoa (anna_siirrot on palattanyt tyhjän siirtovektorin).
	//
	float pisteyta_lopputulos() const
	{
		if (_siirtovuoro == VALKEA)
		{
			// Etsitään valkean kuningas
			int rivi, linja;
			etsi_nappula(wK, rivi, linja);

			// Onko valkean kuningas uhattu?
			if (onko_ruutu_uhattu(rivi, linja, MUSTA))
				return -1000000; // musta on tehnyt matin
			else
				return 0; // patti
		}
		else
		{
			// Etsitään mustan kuningas
			int rivi, linja;
			etsi_nappula(bK, rivi, linja);

			// Onko mustan kuningas uhattu?
			if (onko_ruutu_uhattu(rivi, linja, VALKEA))
				return 1000000; // valkea on tehnyt matin
			else
				return 0; // patti
		}
	}

	// Pisteyttää shakkiaseman heuristisesti.
	float evaluoi() const
	{
		return 1.0f * materiaali() + 0.1f * mobiliteetti();

		// TODO
		// täydentäkää halutessanne uusilla pisteytettävillä aseman piirteillä
	}

	//
	// Palauttaa aseman minimax-arvon. Syvyys määrittää,
	// kuinka monta asekelta syvemmälle pelipuuta käydään läpi.
	//
	// Testaaminen esim. pääohjelmasta:
	//
	// Asema asema;
	// MinimaxArvo arvo = asema.minimax(4);
	// 
	// Nyt tietokoneen siirto saadaan pelattua näin:
	// asema.tee_siirto(arvo._siirto);
	//
	MinimaxArvo minimax(int syvyys)
	{
		// Generoidaan aseman siirrot.
		vector<Siirto> siirrot;
		anna_siirrot(siirrot);

		if (siirrot.size() == 0)
		{
			// Rekursion kantatapaus 1:
			// peli on päättynyt (ei yhtään laillista siirtoa).
			return MinimaxArvo(pisteyta_lopputulos(), Siirto());
		}

		if (syvyys == 0)
		{
			// Rekursion kantatapaus 2:
			// ollaan katkaisusyvyydessä.
			return MinimaxArvo(evaluoi(), Siirto());
		}

		// Siirtoja on jäljellä ja ei olla katkaisusyvyydessä,
		// joten kokeillaan yksitellen mahdollisia siirtoja,
		// ja kutsutaan minimax:a kullekin seuraaja-asemalle.
		// Otetaan paras minimax-arvo talteen (alustetaan
		// paras_arvo mahdollisimman huonoksi siirtovuoroisen
		// pelaajan kannalta).
		float paras_arvo = _siirtovuoro == VALKEA ?
			numeric_limits<float>::lowest() : numeric_limits<float>::max();
		Siirto paras_siirto;
		for (Siirto& s : siirrot)
		{
			Asema uusi = *this;
			uusi.tee_siirto(s);

			// Rekursioasekel: kutsutaan minimax:ia seuraaja-asemalle.
			MinimaxArvo arvo = uusi.minimax(syvyys - 1);

			// Jos saatiin paras arvo, otetaan se talteen.
			if (_siirtovuoro == VALKEA && arvo._arvo > paras_arvo)
			{
				paras_arvo = arvo._arvo;
				paras_siirto = s;
			}
			else if (_siirtovuoro == MUSTA && arvo._arvo < paras_arvo)
			{
				paras_arvo = arvo._arvo;
				paras_siirto = s;
			}
		}

		// Palautetaan paras arvo.
		return MinimaxArvo(paras_arvo, paras_siirto);
	}



	// Laskee materiaalitasapainon (valkean nappuloiden arvo - mustan nappuloiden arvo).
	// Nappuloiden arvot:
	//
	// sotilas		1
	// ratsu		3
	// lähetti		3
	// torni		5
	// daami		9
	//
	float materiaali() const
	{
		// Liitetään nappulatyyppeihin niiden arvot.
		static map<int, float> nappuloiden_arvot = {
			{wP,  1.0f}, {wN,  3.0f}, {wB,  3.0f}, {wR,  5.0f}, {wQ,  9.0f},
			{bP, -1.0f}, {bN, -3.0f}, {bB, -3.0f}, {bR, -5.0f}, {bQ, -9.0f},
			{NA,  0.0f}
		};

		float arvo = 0;
		for (int rivi = 0; rivi < 8; ++rivi)
			for (int linja = 0; linja < 8; ++linja)
			{
				int nappula = _lauta[rivi][linja];
				arvo += nappuloiden_arvot[nappula];
			}
		return arvo;
	}


	// Palauttaa valkean ja mustan (raaka)siirtojen lukumäärien erotuksen.
	float mobiliteetti() const
	{
		vector<Siirto> valkean_siirrot;
		vector<Siirto> mustan_siirrot;

		// Funktion totetus on raskas, koska generoidaan raakasiirtoja.
		// TODO: voisiko optimoida optimoida jotenkin?
		anna_kaikki_raakasiirrot(VALKEA, valkean_siirrot);
		anna_kaikki_raakasiirrot(MUSTA, mustan_siirrot);

		return (float)valkean_siirrot.size() - (float)mustan_siirrot.size();
	}



	// Antaa aseman kaikki raakasiirrot
	void anna_kaikki_raakasiirrot(int pelaaja, std::vector<Siirto>& siirrot) const;

	// Linnoitukset ja ohestalyönnit
	void anna_linnoitukset(int pelaaja, std::vector<Siirto>& siirrot) const
	{
		if (pelaaja == VALKEA)
		{
			// kuningas ja torni eivät liikkuneet, f1 tyhjä, g1 tyhjä,
			// kuningas ei shakissa (e1 ei uhattu), f1 ei uhattu (kuningas ei
			// saa sääntöjen mukaan hypätä uhatun ruudun yli)
			if (_valkea_lyhyt_linna_sallittu && _lauta[7][5] == NA && _lauta[7][6] == NA &&
				!onko_ruutu_uhattu(7, 4, MUSTA) && !onko_ruutu_uhattu(7, 5, MUSTA))
			{
				// Lisätään lyhyt linnoitus
				siirrot.push_back(Siirto(7, 4, 7, 6));

				// TAI: siirrot.push_back(Siirto("e1g1"));
			}
			if (_valkea_pitka_linna_sallittu)
			{
				// TODO: valkean pitkä linnoitus
			}
		}
		else
		{
			// TODO: mustan linnoitukset
		}
	}


	// etsii annetun nappulan (esim. wK) rivin ja linjan
	void etsi_nappula(int nappula, int& rivi, int& linja) const
	{
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				if (_lauta[i][j] == nappula)
				{
					rivi = i;
					linja = j;
					return;
				}
	}


	bool onko_ruutu_uhattu(int rivi, int linja, int uhkaava_pelaaja) const
	{
		// Käydään läpi kaikki vastustajan mahdolliset raakasiirrot ja tarkistetaan uhkaako jokin niistä ruutua.
		vector<Siirto> vastustajan_siirrot;
		anna_kaikki_raakasiirrot(vastustaja(uhkaava_pelaaja), vastustajan_siirrot);

		for (const Siirto& siirto : vastustajan_siirrot) {
			int kohde_rivi = siirto._l_r;
			int kohde_linja = siirto._l_l;

			if (kohde_rivi == rivi && kohde_linja == linja) {
				return true;
			}
		}

		return false;
	}



	// Antaa tornin raakasiirrot (shakeista ei välitetä).
	void anna_tornin_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;

	// Antaa lähetin raakasiirrot (shakeista ei välitetä).
	void anna_lahetin_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;

	// Antaa daamin raakasiirrot (shakeista ei välitetä).
	void anna_daamin_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;

	// Antaa ratsun raakasiirrot (shakeista ei välitetä).
	void anna_ratsun_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;

	// Antaa kuninkaan raakasiirrot (shakeista ei välitetä).
	void anna_kuninkaan_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;

	// Antaa sotilaan raakasiirrot (shakeista ei välitetä, myös ohestalyönti).
	void anna_sotilaan_raakasiirrot(int rivi, int linja, int pelaaja,
		std::vector<Siirto>& siirrot) const;


	/// Anna nappulan mahdolliset raakasiirrot tiettyyn suuntaan.
	///
	/// \param rivi			nappulan lähtörivi
	/// \param linja		nappulan lähtölinja
	/// \param rivi_muutos	rivien koordinaattimuutos (esim. -1 on "ylös")
	/// \param linja_muutos	linjojen koordinaattimuutos (esim. 1 on "oikea")
	/// \param pelaaja		nappulan väri (VALKEA, MUSTA)
	/// \param max_askeleet	maksimiaskeleet ko. suuntaan
	/// \param voi_lyoda	voiko nappula lyödä ko. suuntaan?
	/// \param pakko_lyoda	onko nappulan pakko lyödä liikkuessaan ko. suuntaan?
	/// \param siirrot		siirtovektori joka täytetään mahdollisilla raakasiirroilla
	///
	void anna_raakasiirrot_suunnassa(int rivi, int linja, int rivi_muutos, int linja_muutos,
		int pelaaja, int max_askeleet, bool voi_lyoda, bool pakko_lyoda,
		std::vector<Siirto>& siirrot) const;

	// Laudan nappulat. Indeksointi [rivi][linja], esim.
	//
	// [0][0] : vasen ylänurkka ("a8")
	// [7][0] : vasen alanurkka ("a1")
	// [7][7] : oikea alanurkka ("h1")
	//
	int _lauta[8][8] = {
		{ bR, bN, bB, bQ, bK, bB, bN, bR },
		{ bP, bP, bP, bP, bP, bP, bP, bP },
		{ NA, NA, NA, NA, NA, NA, NA, NA },
		{ NA, NA, NA, NA, NA, NA, NA, NA },
		{ NA, NA, NA, NA, NA, NA, NA, NA },
		{ NA, NA, NA, NA, NA, NA, NA, NA },
		{ wP, wP, wP, wP, wP, wP, wP, wP },
		{ wR, wN, wB, wQ, wK, wB, wN, wR }
	};

	int _siirtovuoro = VALKEA;

	// Kirjanpito siitä, onko kuningas tai torni liikkunut.
	// Asetetaan tarpeen mukaan falseksi.
	bool _valkea_lyhyt_linna_sallittu = true;
	bool _valkea_pitka_linna_sallittu = true;
	bool _musta_lyhyt_linna_sallittu = true;
	bool _musta_pitka_linna_sallittu = true;

	// Ohestalyönti
	int _kaksoisaskel_linjalla = -1;
};


