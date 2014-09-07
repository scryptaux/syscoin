#ifndef ASSET_H
#define ASSET_H

#include "bitcoinrpc.h"
#include "leveldb.h"

class CTransaction;
class CTxOut;
class CValidationState;
class CCoinsViewCache;
class COutPoint;
class CCoins;
class CScript;
class CWalletTx;
class CDiskTxPos;

bool CheckAssetInputs(CBlockIndex *pindex, const CTransaction &tx, CValidationState &state, CCoinsViewCache &inputs,
                     std::map<std::vector<unsigned char>,uint256> &mapTestPool, bool fBlock, bool fMiner, bool fJustCheck);
bool IsAssetMine(const CTransaction& tx);
bool IsAssetMine(const CTransaction& tx, const CTxOut& txout, bool ignore_assetnew = false);
std::string SendAssetMoneyWithInputTx(CScript scriptPubKey, int64 nValue, int64 nNetFee, CWalletTx& wtxIn,
                                     CWalletTx& wtxNew, bool fAskFee, const std::string& txData = "");
bool CreateAssetTransactionWithInputTx(const std::vector<std::pair<CScript, int64> >& vecSend, CWalletTx& wtxIn,
                                      int nTxOut, CWalletTx& wtxNew, CReserveKey& reservekey, int64& nFeeRet, const std::string& txData);
bool DecodeAssetTx(const CTransaction& tx, int& op, int& nOut, std::vector<std::vector<unsigned char> >& vvch, int nHeight);
bool DecodeAssetTx(const CCoins& tx, int& op, int& nOut, std::vector<std::vector<unsigned char> >& vvch, int nHeight);
bool DecodeAssetScript(const CScript& script, int& op, std::vector<std::vector<unsigned char> > &vvch);
bool IsAssetOp(int op);
int IndexOfAssetOutput(const CTransaction& tx);
uint64 GetAssetFeeSubsidy(unsigned int nHeight);
bool GetValueOfAssetTxHash(const uint256 &txHash, std::vector<unsigned char>& vchValue, uint256& hash, int& nHeight);
int GetAssetTxHashHeight(const uint256 txHash);
int GetAssetTxPosHeight(const CDiskTxPos& txPos);
int GetAssetTxPosHeight2(const CDiskTxPos& txPos, int nHeight);
int GetAssetDisplayExpirationDepth(int nHeight);
int64 GetAssetNetworkFee(int seed, int nHeight);
int64 GetAssetNetFee(const CTransaction& tx);
bool InsertAssetFee(CBlockIndex *pindex, uint256 hash, uint64 nValue);
bool ExtractAssetAddress(const CScript& script, std::string& address);

std::string assetFromOp(int op);

extern std::map<std::vector<unsigned char>, uint256> mapMyAssets;
extern std::map<std::vector<unsigned char>, std::set<uint256> > mapAssetPending;

class CBitcoinAddress;

class CAsset {
public:
    std::vector<unsigned char> vchRand;
    std::vector<unsigned char> vchSymbol;
    std::vector<unsigned char> vchTitle;
    std::vector<unsigned char> vchDescription;
    uint64 nTotalQty;
    uint64 nQty;

    uint256 txHash;
    uint64 nHeight;
    uint64 nTime;
    uint256 hash;
    uint64 n;
    uint64 nFee;
    uint64 nOp;

    CAsset() {
        SetNull();
    }

    IMPLEMENT_SERIALIZE (
        READWRITE(vchRand);
        READWRITE(vchSymbol);
        READWRITE(vchTitle);
        READWRITE(vchDescription);
        READWRITE(nTotalQty);
        READWRITE(nQty);
        READWRITE(txHash);
        READWRITE(nHeight);
        READWRITE(nTime);
        READWRITE(hash);
        READWRITE(n);
        READWRITE(nFee);
        READWRITE(nOp);
    )

    void PutToAssetList(std::vector<CAsset> &assetList) {
        for(unsigned int i=0;i<assetList.size();i++) {
            CAsset o = assetList[i];
            if(o.nHeight == nHeight) {
                assetList[i] = *this;
                return;
            }
        }
        assetList.push_back(*this);
    }

    bool GetAssetFromList(const std::vector<CAsset> &assetList) {
        if(assetList.size() == 0) return false;
        for(unsigned int i=0;i<assetList.size();i++) {
            CAsset o = assetList[i];
            if(o.nHeight == nHeight) {
                *this = assetList[i];
                return true;
            }
        }
        *this = assetList.back();
        return false;
    }

    friend bool operator==(const CAsset &a, const CAsset &b) {
        return (
           a.vchRand == b.vchRand
        && a.vchTitle == b.vchTitle
        && a.vchSymbol == b.vchSymbol
        && a.vchDescription == b.vchDescription
        && a.nTotalQty == b.nTotalQty
        && a.nQty == b.nQty
        && a.nFee == b.nFee
        && a.n == b.n
        && a.hash == b.hash
        && a.txHash == b.txHash
        && a.nHeight == b.nHeight
        && a.nTime == b.nTime
        && a.nOp == b.nOp
        );
    }

    CAsset operator=(const CAsset &b) {
        vchRand = b.vchRand;
        vchTitle = b.vchTitle;
        vchSymbol = b.vchSymbol;
        vchDescription = b.vchDescription;
        nTotalQty = b.nTotalQty;
        nQty = b.nQty;
        nFee = b.nFee;
        n = b.n;
        hash = b.hash;
        txHash = b.txHash;
        nHeight = b.nHeight;
        nTime = b.nTime;
        nOp = b.nOp;
        return *this;
    }

    friend bool operator!=(const CAsset &a, const CAsset &b) {
        return !(a == b);
    }

    void SetNull() { 
        nHeight = n = nOp = 0; 
        txHash = hash = 0; 
        nTotalQty = nQty = 0;
        vchRand.clear(); 
        vchSymbol.clear(); 
        vchTitle.clear(); 
        vchDescription.clear(); 
    }
    bool IsNull() const { return (n == 0 && txHash == 0 && hash == 0 && nHeight == 0 && nOp == 0 && vchRand.size() == 0); }

    bool UnserializeFromTx(const CTransaction &tx);
    void SerializeToTx(CTransaction &tx);
    std::string SerializeToString();
};

class CAssetFee {
public:
    uint256 hash;
    uint64 nHeight;
    uint64 nTime;
    uint64 nFee;

    CAssetFee() {
        nTime = 0; nHeight = 0; hash = 0; nFee = 0;
    }

    IMPLEMENT_SERIALIZE (
        READWRITE(hash);
        READWRITE(nHeight);
        READWRITE(nTime);
        READWRITE(nFee);
    )

    friend bool operator==(const CAssetFee &a, const CAssetFee &b) {
        return (
        a.nTime==b.nTime
        && a.hash==b.hash
        && a.nHeight==b.nHeight
        && a.nFee == b.nFee
        );
    }

    CAssetFee operator=(const CAssetFee &b) {
        nTime = b.nTime;
        nFee = b.nFee;
        hash = b.hash;
        nHeight = b.nHeight;
        return *this;
    }

    friend bool operator!=(const CAssetFee &a, const CAssetFee &b) { return !(a == b); }
    void SetNull() { hash = nTime = nHeight = nFee = 0;}
    bool IsNull() const { return (nTime == 0 && nFee == 0 && hash == 0 && nHeight == 0); }
};

class CAssetDB : public CLevelDB {
public:
    CAssetDB(size_t nCacheSize, bool fMemory, bool fWipe) : CLevelDB(GetDataDir() / "assets", nCacheSize, fMemory, fWipe) {}

    bool WriteAsset(const std::vector<unsigned char>& name, std::vector<CAsset>& vtxPos) {
        return Write(make_pair(std::string("asseti"), name), vtxPos);
    }

    bool EraseAsset(const std::vector<unsigned char>& name) {
        return Erase(make_pair(std::string("asseti"), name));
    }

    bool ReadAsset(const std::vector<unsigned char>& name, std::vector<CAsset>& vtxPos) {
        return Read(make_pair(std::string("asseti"), name), vtxPos);
    }

    bool ExistsAsset(const std::vector<unsigned char>& name) {
        return Exists(make_pair(std::string("asseti"), name));
    }

    bool WriteAssetFees(std::vector<CAssetFee>& vtxPos) {
        return Write(make_pair(std::string("asseti"), std::string("assettxf")), vtxPos);
    }

    bool ReadAssetFees(std::vector<CAssetFee>& vtxPos) {
        return Read(make_pair(std::string("asseti"), std::string("assettxf")), vtxPos);
    }

    bool ScanAssets(
            const std::vector<unsigned char>& vchName,
            unsigned int nMax,
            std::vector<std::pair<std::vector<unsigned char>, CAsset> >& assetScan);

    bool ReconstructAssetIndex(CBlockIndex *pindexRescan);
};
extern std::list<CAssetFee> lstAssetFees;


bool GetTxOfAsset(CAssetDB& dbAsset, const std::vector<unsigned char> &vchAsset, CTransaction& tx);

#endif // ASSET_H
