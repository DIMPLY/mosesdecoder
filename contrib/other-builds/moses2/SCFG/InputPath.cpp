/*
 * InputPath.cpp
 *
 *  Created on: 23 Oct 2015
 *      Author: hieu
 */
#include <boost/foreach.hpp>
#include "InputPath.h"
#include "TargetPhrases.h"
#include "ActiveChart.h"
#include "../TranslationModel/PhraseTable.h"
#include "../MemPool.h"

using namespace std;

namespace Moses2
{
namespace SCFG
{

InputPath::InputPath(MemPool &pool, const SubPhrase &subPhrase,
    const Range &range, size_t numPt, const InputPath *prefixPath) :
    InputPathBase(pool, subPhrase, range, numPt, prefixPath)
{
  m_activeChart = pool.Allocate<ActiveChart>(numPt);
  for (size_t i = 0; i < numPt; ++i) {
    ActiveChart &memAddr = m_activeChart[i];
    ActiveChart *obj = new (&memAddr) ActiveChart();
  }
}

InputPath::~InputPath()
{
  // TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream &out, const InputPath &obj)
{
  out << obj.range << " " << obj.subPhrase;
  return out;
}

void InputPath::AddTargetPhrase(const PhraseTable &pt,
    const SCFG::SymbolBind &symbolBind,
    const SCFG::TargetPhraseImpl *tp)
{
  targetPhrases[symbolBind].AddTargetPhrase(*tp);
}

void InputPath::AddActiveChartEntry(size_t ptInd, ActiveChartEntry *chartEntry)
{
  cerr << "added " << range << " " << ptInd << endl;
  ActiveChart &activeChart = m_activeChart[ptInd];
  activeChart.entries.push_back(chartEntry);
}

}
}

