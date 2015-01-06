#pragma once

#include <string>
#include <Util.h>
#include <TranslationOptionList.h>
#include "StatelessFeatureFunction.h"
#include "Classifier.h"
#include "VWFeatureFeature.h"

namespace Moses
{

class VWFeatureDummy : public StatelessFeatureFunction
{
public:
  VWFeatureDummy(const std::string &line)
    :StatelessFeatureFunction(1, line), m_train(false)
  {
    ReadParameters();
    if (m_train) {
      m_trainer = new Discriminative::VWTrainer(m_modelPath);
    } else {
      m_predictorFactory = new Discriminative::VWPredictorFactory(m_modelPath, m_vwOptions);
    }
  }

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }

  void EvaluateInIsolation(const Phrase &source
                , const TargetPhrase &targetPhrase
                , ScoreComponentCollection &scoreBreakdown
                , ScoreComponentCollection &estimatedFutureScore) const
  {}
  
  void EvaluateWithSourceContext(const InputType &input
                , const InputPath &inputPath
                , const TargetPhrase &targetPhrase
                , const StackVec *stackVec
                , ScoreComponentCollection &scoreBreakdown
                , ScoreComponentCollection *estimatedFutureScore = NULL) const
  {}

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
                , const TranslationOptionList &translationOptionList) const
  {
    Discriminative::Classifier *classifier = m_train ? m_trainer : m_predictorFactory->Acquire();
    std::vector<VWFeatureFeature*>& features = VWFeatureFeature::GetFeatures();
    
    TranslationOptionList::const_iterator iterTransOpt;
    for(iterTransOpt = translationOptionList.begin() ;
        iterTransOpt != translationOptionList.end() ; ++iterTransOpt) {
     
      TranslationOption &transOpt = **iterTransOpt;
      for(size_t i = 0; i < features.size(); ++i)
      
        features[i]->DoSomething(input, transOpt.GetInputPath(), transOpt.GetTargetPhrase());
    }
    
    for(iterTransOpt = translationOptionList.begin() ;
        iterTransOpt != translationOptionList.end() ; ++iterTransOpt) {
      TranslationOption &transOpt = **iterTransOpt;
      
      vector<float> newScores(m_numScoreComponents);
      newScores[0] = 1; // Future result of VW
    
      ScoreComponentCollection &scoreBreakDown = transOpt.GetScoreBreakdown();
      scoreBreakDown.PlusEquals(this, newScores);
      
      transOpt.UpdateScore();
    }
  }

  void EvaluateWhenApplied(const Hypothesis& hypo,
                ScoreComponentCollection* accumulator) const
  {}
  
  void EvaluateWhenApplied(const ChartHypothesis &hypo,
                     ScoreComponentCollection* accumulator) const
  {}


  void SetParameter(const std::string& key, const std::string& value)
  {
    if (key == "train") {
      m_train = Scan<bool>(value);
    } else if (key == "model") {
      m_modelPath = value;
    } else if (key == "vw-options") {
      m_vwOptions = value;
    } else {
      StatelessFeatureFunction::SetParameter(key, value);
    }
  }

private:
  bool m_train; // false means predict
  std::string m_modelPath;
  std::string m_vwOptions;
  Discriminative::Classifier *m_trainer;
  Discriminative::VWPredictorFactory *m_predictorFactory;
};

}
