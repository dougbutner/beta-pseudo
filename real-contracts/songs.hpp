// --- Songs Helper Contract for ups.cxc --- \\

struct song {
  string title;
  vector<string> links;
  vector<double> geoloc; // CHECK this is a real 
  enum<uint8_t> genre;
  enum<uint8_t> mood;
  enum<uint8_t> format;
  string googleid;
};

enum genres: uint8_t {
  NULL = 0,
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

enum moods: uint8_t {
  NULL = 0,
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

enum formats: uint8_t {
  NULL = 0,
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
