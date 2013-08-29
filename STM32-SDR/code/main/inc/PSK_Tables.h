/* PSK tables
 Based on code by Moe Weatley; changes may have been made
 for the STM32-SDR project. Original license below:

     ////////////////////////////////////////////////////////////////////
	 PSK31Core Library for transmission and reception of PSK31 signals
		using a PC soundcard  or .wav files.
					   Copyright 2000, Moe Wheatley, AE4JY

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	 http://www.gnu.org/copyleft/lesser.html
	////////////////////////////////////////////////////////////////////
*/
#ifndef PSK_TABLES_H_
#define PSK_TABLES_H_

/* bit sync half distance table                                  */
/* index any position 0 to 15 and it returns a position halfway from the index */
const int HALF_TBL[16] = {
		7,	/* 0           */
		8,	/* 1           */
		9,	/* 2           */
		10,	/* 3           */
		11,	/* 4           */
		12,	/* 5           */
		13,	/* 6           */
		14,	/* 7           */
		0,	/* 8           */
		1,	/* 9           */
		2,	/* 10          */
		3,	/* 11          */
		4,	/* 12          */
		5,	/* 13          */
		6,	/* 14          */
		7	/* 15          */
};


/* each character is separated by two zeros. The bits are sent msbit first. */
const unsigned int VARICODE_TABLE[256] = {
		0xAAC0,	/* ASCII =   0	1010101011                    */
		0xB6C0,	/* ASCII =   1	1011011011                    */
		0xBB40,	/* ASCII =   2	1011101101                    */
		0xDDC0,	/* ASCII =   3	1101110111                    */
		0xBAC0,	/* ASCII =   4	1011101011                    */
		0xD7C0,	/* ASCII =   5	1101011111                    */
		0xBBC0,	/* ASCII =   6	1011101111                    */
		0xBF40,	/* ASCII =   7	1011111101                    */
		0xBFC0,	/* ASCII =   8	1011111111                    */
		0xEF00,	/* ASCII =   9	11101111                      */
		0xE800,	/* ASCII =  10	11101                         */
		0xDBC0,	/* ASCII =  11	1101101111                    */
		0xB740,	/* ASCII =  12	1011011101                    */
		0xF800,	/* ASCII =  13	11111                         */
		0xDD40,	/* ASCII =  14	1101110101                    */
		0xEAC0,	/* ASCII =  15	1110101011                    */
		0xBDC0,	/* ASCII =  16	1011110111                    */
		0xBD40,	/* ASCII =  17	1011110101                    */
		0xEB40,	/* ASCII =  18	1110101101                    */
		0xEBC0,	/* ASCII =  19	1110101111                    */
		0xD6C0,	/* ASCII =  20	1101011011                    */
		0xDAC0,	/* ASCII =  21	1101101011                    */
		0xDB40,	/* ASCII =  22	1101101101                    */
		0xD5C0,	/* ASCII =  23	1101010111                    */
		0xDEC0,	/* ASCII =  24	1101111011                    */
		0xDF40,	/* ASCII =  25	1101111101                    */
		0xEDC0,	/* ASCII =  26	1110110111                    */
		0xD540,	/* ASCII =  27	1101010101                    */
		0xD740,	/* ASCII =  28	1101011101                    */
		0xEEC0,	/* ASCII =  29	1110111011                    */
		0xBEC0,	/* ASCII =  30	1011111011                    */
		0xDFC0,	/* ASCII =  31	1101111111                    */
		0x8000,	/* ASCII = ' '	1                             */
		0xFF80,	/* ASCII = '!'	111111111                     */
		0xAF80,	/* ASCII = '"'	101011111                     */
		0xFA80,	/* ASCII = '#'	111110101                     */
		0xED80,	/* ASCII = '$'	111011011                     */
		0xB540,	/* ASCII = '%'	1011010101                    */
		0xAEC0,	/* ASCII = '&'	1010111011                    */
		0xBF80,	/* ASCII = '''	101111111                     */
		0xFB00,	/* ASCII = '('	11111011                      */
		0xF700,	/* ASCII = ')'	11110111                      */
		0xB780,	/* ASCII = '*'	101101111                     */
		0xEF80,	/* ASCII = '+'	111011111                     */
		0xEA00,	/* ASCII = ','	1110101                       */
		0xD400,	/* ASCII = '-'	110101                        */
		0xAE00,	/* ASCII = '.'	1010111                       */
		0xD780,	/* ASCII = '/'	110101111                     */
		0xB700,	/* ASCII = '0'	10110111                      */
		0xBD00,	/* ASCII = '1'	10111101                      */
		0xED00,	/* ASCII = '2'	11101101                      */
		0xFF00,	/* ASCII = '3'	11111111                      */
		0xBB80,	/* ASCII = '4'	101110111                     */
		0xAD80,	/* ASCII = '5'	101011011                     */
		0xB580,	/* ASCII = '6'	101101011                     */
		0xD680,	/* ASCII = '7'	110101101                     */
		0xD580,	/* ASCII = '8'	110101011                     */
		0xDB80,	/* ASCII = '9'	110110111                     */
		0xF500,	/* ASCII = ':'	11110101                      */
		0xDE80,	/* ASCII = ';'	110111101                     */
		0xF680,	/* ASCII = '<'	111101101                     */
		0xAA00,	/* ASCII = '='	1010101                       */
		0xEB80,	/* ASCII = '>'	111010111                     */
		0xABC0,	/* ASCII = '?'	1010101111                    */
		0xAF40,	/* ASCII = '@'	1010111101                    */
		0xFA00,	/* ASCII = 'A'	1111101                       */
		0xEB00,	/* ASCII = 'B'	11101011                      */
		0xAD00,	/* ASCII = 'C'	10101101                      */
		0xB500,	/* ASCII = 'D'	10110101                      */
		0xEE00,	/* ASCII = 'E'	1110111                       */
		0xDB00,	/* ASCII = 'F'	11011011                      */
		0xFD00,	/* ASCII = 'G'	11111101                      */
		0xAA80,	/* ASCII = 'H'	101010101                     */
		0xFE00,	/* ASCII = 'I'	1111111                       */
		0xFE80,	/* ASCII = 'J'	111111101                     */
		0xBE80,	/* ASCII = 'K'	101111101                     */
		0xD700,	/* ASCII = 'L'	11010111                      */
		0xBB00,	/* ASCII = 'M'	10111011                      */
		0xDD00,	/* ASCII = 'N'	11011101                      */
		0xAB00,	/* ASCII = 'O'	10101011                      */
		0xD500,	/* ASCII = 'P'	11010101                      */
		0xEE80,	/* ASCII = 'Q'	111011101                     */
		0xAF00,	/* ASCII = 'R'	10101111                      */
		0xDE00,	/* ASCII = 'S'	1101111                       */
		0xDA00,	/* ASCII = 'T'	1101101                       */
		0xAB80,	/* ASCII = 'U'	101010111                     */
		0xDA80,	/* ASCII = 'V'	110110101                     */
		0xAE80,	/* ASCII = 'W'	101011101                     */
		0xBA80,	/* ASCII = 'X'	101110101                     */
		0xBD80,	/* ASCII = 'Y'	101111011                     */
		0xAB40,	/* ASCII = 'Z'	1010101101                    */
		0xFB80,	/* ASCII = '['	111110111                     */
		0xF780,	/* ASCII = '\'	111101111                     */
		0xFD80,	/* ASCII = ']'	111111011                     */
		0xAFC0,	/* ASCII = '^'	1010111111                    */
		0xB680,	/* ASCII = '_'	101101101                     */
		0xB7C0,	/* ASCII = '`'	1011011111                    */
		0xB000,	/* ASCII = 'a'	1011                          */
		0xBE00,	/* ASCII = 'b'	1011111                       */
		0xBC00,	/* ASCII = 'c'	101111                        */
		0xB400,	/* ASCII = 'd'	101101                        */
		0xC000,	/* ASCII = 'e'	11                            */
		0xF400,	/* ASCII = 'f'	111101                        */
		0xB600,	/* ASCII = 'g'	1011011                       */
		0xAC00,	/* ASCII = 'h'	101011                        */
		0xD000,	/* ASCII = 'i'	1101                          */
		0xF580,	/* ASCII = 'j'	111101011                     */
		0xBF00,	/* ASCII = 'k'	10111111                      */
		0xD800,	/* ASCII = 'l'	11011                         */
		0xEC00,	/* ASCII = 'm'	111011                        */
		0xF000,	/* ASCII = 'n'	1111                          */
		0xE000,	/* ASCII = 'o'	111                           */
		0xFC00,	/* ASCII = 'p'	111111                        */
		0xDF80,	/* ASCII = 'q'	110111111                     */
		0xA800,	/* ASCII = 'r'	10101                         */
		0xB800,	/* ASCII = 's'	10111                         */
		0xA000,	/* ASCII = 't'	101                           */
		0xDC00,	/* ASCII = 'u'	110111                        */
		0xF600,	/* ASCII = 'v'	1111011                       */
		0xD600,	/* ASCII = 'w'	1101011                       */
		0xDF00,	/* ASCII = 'x'	11011111                      */
		0xBA00,	/* ASCII = 'y'	1011101                       */
		0xEA80,	/* ASCII = 'z'	111010101                     */
		0xADC0,	/* ASCII = '{'	1010110111                    */
		0xDD80,	/* ASCII = '|'	110111011                     */
		0xAD40,	/* ASCII = '}'	1010110101                    */
		0xB5C0,	/* ASCII = '~'	1011010111                    */
		0xED40,	/* ASCII = 127	1110110101                    */
		0xEF40,	/* ASCII = 128	1110111101                    */
		0xEFC0,	/* ASCII = 129	1110111111                    */
		0xF540,	/* ASCII = 130	1111010101                    */
		0xF5C0,	/* ASCII = 131	1111010111                    */
		0xF6C0,	/* ASCII = 132	1111011011                    */
		0xF740,	/* ASCII = 133	1111011101                    */
		0xF7C0,	/* ASCII = 134	1111011111                    */
		0xFAC0,	/* ASCII = 135	1111101011                    */
		0xFB40,	/* ASCII = 136	1111101101                    */
		0xFBC0,	/* ASCII = 137	1111101111                    */
		0xFD40,	/* ASCII = 138	1111110101                    */
		0xFDC0,	/* ASCII = 139	1111110111                    */
		0xFEC0,	/* ASCII = 140	1111111011                    */
		0xFF40,	/* ASCII = 141	1111111101                    */
		0xFFC0,	/* ASCII = 142	1111111111                    */
		0xAAA0,	/* ASCII = 143	10101010101                   */
		0xAAE0,	/* ASCII = 144	10101010111                   */
		0xAB60,	/* ASCII = 145	10101011011                   */
		0xABA0,	/* ASCII = 146	10101011101                   */
		0xABE0,	/* ASCII = 147	10101011111                   */
		0xAD60,	/* ASCII = 148	10101101011                   */
		0xADA0,	/* ASCII = 149	10101101101                   */
		0xADE0,	/* ASCII = 150	10101101111                   */
		0xAEA0,	/* ASCII = 151	10101110101                   */
		0xAEE0,	/* ASCII = 152	10101110111                   */
		0xAF60,	/* ASCII = 153	10101111011                   */
		0xAFA0,	/* ASCII = 154	10101111101                   */
		0xAFE0,	/* ASCII = 155	10101111111                   */
		0xB560,	/* ASCII = 156	10110101011                   */
		0xB5A0,	/* ASCII = 157	10110101101                   */
		0xB5E0,	/* ASCII = 158	10110101111                   */
		0xB6A0,	/* ASCII = 159	10110110101                   */
		0xB6E0,	/* ASCII = 160	10110110111                   */
		0xB760,	/* ASCII = 161	10110111011                   */
		0xB7A0,	/* ASCII = 162	10110111101                   */
		0xB7E0,	/* ASCII = 163	10110111111                   */
		0xBAA0,	/* ASCII = 164	10111010101                   */
		0xBAE0,	/* ASCII = 165	10111010111                   */
		0xBB60,	/* ASCII = 166	10111011011                   */
		0xBBA0,	/* ASCII = 167	10111011101                   */
		0xBBE0,	/* ASCII = 168	10111011111                   */
		0xBD60,	/* ASCII = 169	10111101011                   */
		0xBDA0,	/* ASCII = 170	10111101101                   */
		0xBDE0,	/* ASCII = 171	10111101111                   */
		0xBEA0,	/* ASCII = 172	10111110101                   */
		0xBEE0,	/* ASCII = 173	10111110111                   */
		0xBF60,	/* ASCII = 174	10111111011                   */
		0xBFA0,	/* ASCII = 175	10111111101                   */
		0xBFE0,	/* ASCII = 176	10111111111                   */
		0xD560,	/* ASCII = 177	11010101011                   */
		0xD5A0,	/* ASCII = 178	11010101101                   */
		0xD5E0,	/* ASCII = 179	11010101111                   */
		0xD6A0,	/* ASCII = 180	11010110101                   */
		0xD6E0,	/* ASCII = 181	11010110111                   */
		0xD760,	/* ASCII = 182	11010111011                   */
		0xD7A0,	/* ASCII = 183	11010111101                   */
		0xD7E0,	/* ASCII = 184	11010111111                   */
		0xDAA0,	/* ASCII = 185	11011010101                   */
		0xDAE0,	/* ASCII = 186	11011010111                   */
		0xDB60,	/* ASCII = 187	11011011011                   */
		0xDBA0,	/* ASCII = 188	11011011101                   */
		0xDBE0,	/* ASCII = 189	11011011111                   */
		0xDD60,	/* ASCII = 190	11011101011                   */
		0xDDA0,	/* ASCII = 191	11011101101                   */
		0xDDE0,	/* ASCII = 192	11011101111                   */
		0xDEA0,	/* ASCII = 193	11011110101                   */
		0xDEE0,	/* ASCII = 194	11011110111                   */
		0xDF60,	/* ASCII = 195	11011111011                   */
		0xDFA0,	/* ASCII = 196	11011111101                   */
		0xDFE0,	/* ASCII = 197	11011111111                   */
		0xEAA0,	/* ASCII = 198	11101010101                   */
		0xEAE0,	/* ASCII = 199	11101010111                   */
		0xEB60,	/* ASCII = 200	11101011011                   */
		0xEBA0,	/* ASCII = 201	11101011101                   */
		0xEBE0,	/* ASCII = 202	11101011111                   */
		0xED60,	/* ASCII = 203	11101101011                   */
		0xEDA0,	/* ASCII = 204	11101101101                   */
		0xEDE0,	/* ASCII = 205	11101101111                   */
		0xEEA0,	/* ASCII = 206	11101110101                   */
		0xEEE0,	/* ASCII = 207	11101110111                   */
		0xEF60,	/* ASCII = 208	11101111011                   */
		0xEFA0,	/* ASCII = 209	11101111101                   */
		0xEFE0,	/* ASCII = 210	11101111111                   */
		0xF560,	/* ASCII = 211	11110101011                   */
		0xF5A0,	/* ASCII = 212	11110101101                   */
		0xF5E0,	/* ASCII = 213	11110101111                   */
		0xF6A0,	/* ASCII = 214	11110110101                   */
		0xF6E0,	/* ASCII = 215	11110110111                   */
		0xF760,	/* ASCII = 216	11110111011                   */
		0xF7A0,	/* ASCII = 217	11110111101                   */
		0xF7E0,	/* ASCII = 218	11110111111                   */
		0xFAA0,	/* ASCII = 219	11111010101                   */
		0xFAE0,	/* ASCII = 220	11111010111                   */
		0xFB60,	/* ASCII = 221	11111011011                   */
		0xFBA0,	/* ASCII = 222	11111011101                   */
		0xFBE0,	/* ASCII = 223	11111011111                   */
		0xFD60,	/* ASCII = 224	11111101011                   */
		0xFDA0,	/* ASCII = 225	11111101101                   */
		0xFDE0,	/* ASCII = 226	11111101111                   */
		0xFEA0,	/* ASCII = 227	11111110101                   */
		0xFEE0,	/* ASCII = 228	11111110111                   */
		0xFF60,	/* ASCII = 229	11111111011                   */
		0xFFA0,	/* ASCII = 230	11111111101                   */
		0xFFE0,	/* ASCII = 231	11111111111                   */
		0xAAB0,	/* ASCII = 232	101010101011                  */
		0xAAD0,	/* ASCII = 233	101010101101                  */
		0xAAF0,	/* ASCII = 234	101010101111                  */
		0xAB50,	/* ASCII = 235	101010110101                  */
		0xAB70,	/* ASCII = 236	101010110111                  */
		0xABB0,	/* ASCII = 237	101010111011                  */
		0xABD0,	/* ASCII = 238	101010111101                  */
		0xABF0,	/* ASCII = 239	101010111111                  */
		0xAD50,	/* ASCII = 240	101011010101                  */
		0xAD70,	/* ASCII = 241	101011010111                  */
		0xADB0,	/* ASCII = 242	101011011011                  */
		0xADD0,	/* ASCII = 243	101011011101                  */
		0xADF0,	/* ASCII = 244	101011011111                  */
		0xAEB0,	/* ASCII = 245	101011101011                  */
		0xAED0,	/* ASCII = 246	101011101101                  */
		0xAEF0,	/* ASCII = 247	101011101111                  */
		0xAF50,	/* ASCII = 248	101011110101                  */
		0xAF70,	/* ASCII = 249	101011110111                  */
		0xAFB0,	/* ASCII = 250	101011111011                  */
		0xAFD0,	/* ASCII = 251	101011111101                  */
		0xAFF0,	/* ASCII = 252	101011111111                  */
		0xB550,	/* ASCII = 253	101101010101                  */
		0xB570,	/* ASCII = 254	101101010111                  */
		0xB5B0	/* ASCII = 255	101101011011                  */
};

#endif /* PSK_TABLES_H_ */


