// --- Songs Helper Contract for ups.cxc --- \\

using namespace std;
using namespace eosio;
using std::string;

struct song {
  string title;
  vector<string> links;
  vector<double> geoloc; // CHECK this is a real 
  enum<uint8_t> genre;
  enum<uint8_t> mood;
  enum<uint8_t> format;
  string atomictempid;
};


enum genre : uint8_t {
  DEFGENRE, // NOTE null->none CONVERSION NEEDED `NONE` is used here because NULL is a reserved word 
  CONSCIOUS,
  TRANCE,
  HOUSE,
  TECHNO,
  ELECTRONIC,
  EXPERIMENTAL,
  EDM,
  BLUES,
  SOUL,
  FUNK,
  FUSION,
  LOFI,
  JAZZ,
  CULTURAL,
  REGGAE,
  POP,
  CINEMATIC,
  CLASSICAL,
  HIPHOP,
  RAP,
  RNB,
  REGGAETON,
  ROCK,
  PUNK,
  INDIE,
  TRIPHOP,
  ALTERNATIVE,
  METAL,
  LATIN,
  SALSA,
  SAMBA,
  BOSSANOVA,
  SOUNDSCAPE,
  SPOKENWORD,
  VOCAL,
  CHORAL,
  BALLAD,
  BLUEGRASS,
  COUNTRY
};

enum mood : uint8_t {
  DEFMOOD,// NOTE null->none CONVERSION NEEDED `NONE` is used here because NULL is a reserved word
  CHILL,
  SPIRITUAL,
  HAPPY,
  DANCE,
  PARTY,
  HIGH,
  ENERGIZING,
  UPLIFTING,
  FOCUSED,
  EMOTIONAL,
  ANGRY,
  WORKOUT,
  PROVOCATIVE,
  BALLIN,
  HEAVY,
  LIGHT,
  SLEEPY,
  INTROSPECTIVE,
  TRIPPY,
  MAGICAL,
  ROMANTIC
};

enum format : uint8_t {
  DEFFORMAT,
  NFT,
  MUSICVIDEO,
  SONG,
  ALBUM,
  COVER,
  INSTRUMENTAL,
  INSTRUMENTALALBUM,
  LIVE,
  LIVESET,
  REMIX,
  PLAYLIST,
  FREESTYLE,
  SNIPPET,
  SKIT
};



/*/
enum genre: uint8_t {
  NONE = 0, // NOTE null->none CONVERSION NEEDED `NONE` is used here because NULL is a reserved word 
  CONSCIOUS = 1,
  TRANCE = 2,
  HOUSE = 3,
  TECHNO = 4,
  ELECTRONIC = 5,
  EXPERIMENTAL = 6,
  EDM = 7,
  BLUES = 8,
  SOUL = 9,
  FUNK = 10,
  FUSION = 11,
  LOFI = 12,
  JAZZ = 13,
  CULTURAL = 14,
  REGGAE= 15,
  POP = 16,
  CINEMATIC = 17,
  CLASSICAL = 18,
  HIPHOP = 19,
  RAP = 20,
  RNB = 21,
  REGGAETON = 22,
  ROCK = 23,
  PUNK = 24,
  INDIE = 25,
  TRIPHOP = 26,
  ALTERNATIVE = 27,
  METAL = 28,
  LATIN = 29,
  SALSA = 30,
  SAMBA = 31,
  BOSSANOVA = 32,
  SOUNDSCAPE = 33,
  SPOKENWORD = 34,
  VOCAL = 35,
  CHORAL = 36,
  BALLAD = 37,
  BLUEGRASS = 38,
  COUNTRY = 39
};

enum mood: uint8_t {
  //NONE = 0,// NOTE null->none CONVERSION NEEDED `NONE` is used here because NULL is a reserved word
  CHILL = 1,
  SPIRITUAL = 2,
  HAPPY = 3,
  DANCE = 4,
  PARTY= 5,
  HIGH = 6,
  ENERGIZING = 7,
  UPLIFTING = 8,
  FOCUSED = 9,
  EMOTIONAL = 10,
  ANGRY = 11,
  WORKOUT = 12,
  PROVOCATIVE = 13,
  BALLIN = 14,
  HEAVY = 15,
  LIGHT = 16,
  SLEEPY = 17,
  INTROSPECTIVE = 18,
  TRIPPY = 19,
  MAGICAL = 20,
  ROMANTIC = 21,
};

enum format: uint8_t {
  //NONE = 0,
  NFT = 1,
  MUSICVIDEO = 2,
  SONG = 3,
  ALBUM = 4,
  COVER = 5,
  INSTRUMENTAL = 6,
  INSTRUMENTALALBUM = 7,
  LIVE = 8,
  LIVESET = 9,
  REMIX = 10,
  PLAYLIST= 11,
  FREESTYLE = 12,
  SNIPPET = 13,
  SKIT = 14
};

/*/