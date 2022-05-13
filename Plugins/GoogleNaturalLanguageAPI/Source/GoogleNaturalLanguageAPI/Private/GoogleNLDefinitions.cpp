// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#include "GoogleNLDefinitions.h"


GoogleNaturalLanguageValueMapping::GoogleNaturalLanguageValueMapping()
{
    // doc type
    documentType.Add(ENLDocumentType::TYPE_UNSPECIFIED, TEXT("TYPE_UNSPECIFIED"));
    documentType.Add(ENLDocumentType::PLAIN_TEXT, TEXT("PLAIN_TEXT"));
    documentType.Add(ENLDocumentType::HTML, TEXT("HTML"));

    // language codes
    languagCodes.Add(ENLLanguage::ARABIC, TEXT("ar"));
    languagCodes.Add(ENLLanguage::CHINISE_SIMPLIFIED, TEXT("zh"));
    languagCodes.Add(ENLLanguage::CHINISE_TRADITIONAL, TEXT("zh-Hant"));
    languagCodes.Add(ENLLanguage::DUTCH, TEXT("nl"));
    languagCodes.Add(ENLLanguage::ENGLISH, TEXT("en"));
    languagCodes.Add(ENLLanguage::FRENCH, TEXT("fr"));
    languagCodes.Add(ENLLanguage::GERMAN, TEXT("de"));
    languagCodes.Add(ENLLanguage::INDONESIAN, TEXT("id"));
    languagCodes.Add(ENLLanguage::ITALIAN, TEXT("it"));
    languagCodes.Add(ENLLanguage::JAPANESE, TEXT("ja"));
    languagCodes.Add(ENLLanguage::KOREAN, TEXT("ko"));
    languagCodes.Add(ENLLanguage::POLISH, TEXT("pl"));
    languagCodes.Add(ENLLanguage::PORTUGUESE, TEXT("pt"));
    languagCodes.Add(ENLLanguage::RUSSIAN, TEXT("ru"));
    languagCodes.Add(ENLLanguage::SPANISH, TEXT("es"));
    languagCodes.Add(ENLLanguage::THAI, TEXT("th"));
    languagCodes.Add(ENLLanguage::TURKISH, TEXT("tr"));
    languagCodes.Add(ENLLanguage::VIETNAMESE, TEXT("vi"));
    languagCodes.Add(ENLLanguage::LANGUAGE_UNKNOWN, TEXT(""));

    // encoding types
    encodingTypes.Add(ENLEncodingType::NONE, TEXT("NONE"));
    encodingTypes.Add(ENLEncodingType::UTF8, TEXT("UTF8"));
    encodingTypes.Add(ENLEncodingType::UTF16, TEXT("UTF16"));
    encodingTypes.Add(ENLEncodingType::UTF32, TEXT("UTF32"));

    // entity types
    entityTypes.Add(ENLEntityType::UNKNOWN, TEXT("UNKNOWN"));
    entityTypes.Add(ENLEntityType::PERSON, TEXT("PERSON"));
    entityTypes.Add(ENLEntityType::LOCATION, TEXT("LOCATION"));
    entityTypes.Add(ENLEntityType::ORGANIZATION, TEXT("ORGANIZATION"));
    entityTypes.Add(ENLEntityType::EVENT, TEXT("EVENT"));
    entityTypes.Add(ENLEntityType::WORK_OF_ART, TEXT("WORK_OF_ART"));
    entityTypes.Add(ENLEntityType::CONSUMER_GOOD, TEXT("CONSUMER_GOOD"));
    entityTypes.Add(ENLEntityType::OTHER, TEXT("OTHER"));
    entityTypes.Add(ENLEntityType::PHONE_NUMBER, TEXT("PHONE_NUMBER"));
    entityTypes.Add(ENLEntityType::ADDRESS, TEXT("ADDRESS"));
    entityTypes.Add(ENLEntityType::DATE, TEXT("DATE"));
    entityTypes.Add(ENLEntityType::NUMBER, TEXT("NUMBER"));
    entityTypes.Add(ENLEntityType::PRICE, TEXT("PRICE"));

    // mention types
    mentionTypes.Add(ENLMentionType::TYPE_UNKNOWN, TEXT("TYPE_UNKNOWN"));
    mentionTypes.Add(ENLMentionType::PROPER, TEXT("PROPER"));
    mentionTypes.Add(ENLMentionType::COMMON, TEXT("COMMON"));

    // tag types
    tagTypes.Add(ENLTag::UNKNOWN, TEXT("UNKNOWN"));
    tagTypes.Add(ENLTag::ADJ, TEXT("ADJ"));
    tagTypes.Add(ENLTag::ADP, TEXT("ADP"));
    tagTypes.Add(ENLTag::ADV, TEXT("ADV"));
    tagTypes.Add(ENLTag::CONJ, TEXT("CONJ"));
    tagTypes.Add(ENLTag::DET, TEXT("DET"));
    tagTypes.Add(ENLTag::NOUN, TEXT("NOUN"));
    tagTypes.Add(ENLTag::NUM, TEXT("NUM"));
    tagTypes.Add(ENLTag::PRON, TEXT("PRON"));
    tagTypes.Add(ENLTag::PRT, TEXT("PRT"));
    tagTypes.Add(ENLTag::PUNCT, TEXT("PUNCT"));
    tagTypes.Add(ENLTag::VERB, TEXT("VERB"));
    tagTypes.Add(ENLTag::X, TEXT("X"));
    tagTypes.Add(ENLTag::AFFIX, TEXT("AFFIX"));

    // aspect types
    aspectTypes.Add(ENLAspect::ASPECT_UNKNOWN, TEXT("ASPECT_UNKNOWN"));
    aspectTypes.Add(ENLAspect::PERFECTIVE, TEXT("PERFECTIVE"));
    aspectTypes.Add(ENLAspect::IMPERFECTIVE, TEXT("IMPERFECTIVE"));
    aspectTypes.Add(ENLAspect::PROGRESSIVE, TEXT("PROGRESSIVE"));

    // case types
    caseTypes.Add(ENLCase::CASE_UNKNOWN, TEXT("CASE_UNKNOWN"));
    caseTypes.Add(ENLCase::ACCUSATIVE, TEXT("ACCUSATIVE"));
    caseTypes.Add(ENLCase::ADVERBIAL, TEXT("ADVERBIAL"));
    caseTypes.Add(ENLCase::COMPLEMENTIVE, TEXT("COMPLEMENTIVE"));
    caseTypes.Add(ENLCase::DATIVE, TEXT("DATIVE"));
    caseTypes.Add(ENLCase::GENITIVE, TEXT("GENITIVE"));
    caseTypes.Add(ENLCase::INSTRUMENTAL, TEXT("INSTRUMENTAL"));
    caseTypes.Add(ENLCase::LOCATIVE, TEXT("LOCATIVE"));
    caseTypes.Add(ENLCase::NOMINATIVE, TEXT("NOMINATIVE"));
    caseTypes.Add(ENLCase::OBLIQUE, TEXT("OBLIQUE"));
    caseTypes.Add(ENLCase::PARTITIVE, TEXT("PARTITIVE"));
    caseTypes.Add(ENLCase::PREPOSITIONAL, TEXT("PREPOSITIONAL"));
    caseTypes.Add(ENLCase::REFLEXIVE_CASE, TEXT("REFLEXIVE_CASE"));
    caseTypes.Add(ENLCase::RELATIVE_CASE, TEXT("RELATIVE_CASE"));
    caseTypes.Add(ENLCase::VOCATIVE, TEXT("VOCATIVE"));

    // form types
    formTypes.Add(ENLForm::FORM_UNKNOWN, TEXT("FORM_UNKNOWN"));
    formTypes.Add(ENLForm::ADNOMIAL, TEXT("ADNOMIAL"));
    formTypes.Add(ENLForm::AUXILIARY, TEXT("AUXILIARY"));
    formTypes.Add(ENLForm::COMPLEMENTIZER, TEXT("COMPLEMENTIZER"));
    formTypes.Add(ENLForm::FINAL_ENDING, TEXT("FINAL_ENDING"));
    formTypes.Add(ENLForm::GERUND, TEXT("GERUND"));
    formTypes.Add(ENLForm::REALIS, TEXT("REALIS"));
    formTypes.Add(ENLForm::IRREALIS, TEXT("IRREALIS"));
    formTypes.Add(ENLForm::SHORT, TEXT("SHORT"));
    formTypes.Add(ENLForm::LONG, TEXT("LONG"));
    formTypes.Add(ENLForm::ORDER, TEXT("ORDER"));
    formTypes.Add(ENLForm::SPECIFIC, TEXT("SPECIFIC"));

    // gender types
    genderTypes.Add(ENLGender::GENDER_UNKNOWN, TEXT("GENDER_UNKNOWN"));
    genderTypes.Add(ENLGender::FEMININE, TEXT("FEMININE"));
    genderTypes.Add(ENLGender::MASCULINE, TEXT("MASCULINE"));
    genderTypes.Add(ENLGender::NEUTER, TEXT("NEUTER"));

    // moodTypes
    moodTypes.Add(ENLMood::MOOD_UNKNOWN, TEXT("MOOD_UNKNOWN"));
    moodTypes.Add(ENLMood::CONDITIONAL_MOOD, TEXT("CONDITIONAL_MOOD"));
    moodTypes.Add(ENLMood::IMPERATIVE, TEXT("IMPERATIVE"));
    moodTypes.Add(ENLMood::INDICATIVE, TEXT("INDICATIVE"));
    moodTypes.Add(ENLMood::INTERROGATIVE, TEXT("INTERROGATIVE"));
    moodTypes.Add(ENLMood::JUSSIVE, TEXT("JUSSIVE"));
    moodTypes.Add(ENLMood::SUBJUNCTIVE, TEXT("SUBJUNCTIVE"));

    // number types
    numberTypes.Add(ENLNumber::NUMBER_UNKNOWN, TEXT("NUMBER_UNKNOWN"));
    numberTypes.Add(ENLNumber::SINGULAR, TEXT("SINGULAR"));
    numberTypes.Add(ENLNumber::PLURAL, TEXT("PLURAL"));
    numberTypes.Add(ENLNumber::DUAL, TEXT("DUAL"));

    // person types
    personTypes.Add(ENLPerson::PERSON_UNKNOWN, TEXT("PERSON_UNKNOWN"));
    personTypes.Add(ENLPerson::FIRST, TEXT("FIRST"));
    personTypes.Add(ENLPerson::SECOND, TEXT("SECOND"));
    personTypes.Add(ENLPerson::THIRD, TEXT("THIRD"));
    personTypes.Add(ENLPerson::REFLEXIVE_PERSON, TEXT("REFLEXIVE_PERSON"));

    // proper types
    properTypes.Add(ENLProper::PROPER_UNKNOWN, TEXT("PROPER_UNKNOWN"));
    properTypes.Add(ENLProper::PROPER, TEXT("PROPER"));
    properTypes.Add(ENLProper::NOT_PROPER, TEXT("NOT_PROPER"));

    // reciprocity types
    reciprocityTypes.Add(ENLReciprocity::RECIPROCITY_UNKNOWN, TEXT("RECIPROCITY_UNKNOWN"));
    reciprocityTypes.Add(ENLReciprocity::RECIPROCAL, TEXT("RECIPROCAL"));
    reciprocityTypes.Add(ENLReciprocity::NON_RECIPROCAL, TEXT("NON_RECIPROCAL"));

    // tense types
    tenseTypes.Add(ENLTense::TENSE_UNKNOWN, TEXT("TENSE_UNKNOWN"));
    tenseTypes.Add(ENLTense::CONDITIONAL_TENSE, TEXT("CONDITIONAL_TENSE"));
    tenseTypes.Add(ENLTense::FUTURE, TEXT("FUTURE"));
    tenseTypes.Add(ENLTense::PAST, TEXT("PAST"));
    tenseTypes.Add(ENLTense::PRESENT, TEXT("PRESENT"));
    tenseTypes.Add(ENLTense::IMPERFECT, TEXT("IMPERFECT"));
    tenseTypes.Add(ENLTense::PLUPERFECT, TEXT("PLUPERFECT"));

    // voice types
    voiceTypes.Add(ENLVoice::VOICE_UNKNOWN, TEXT("VOICE_UNKNOWN"));
    voiceTypes.Add(ENLVoice::ACTIVE, TEXT("ACTIVE"));
    voiceTypes.Add(ENLVoice::CAUSATIVE, TEXT("CAUSATIVE"));
    voiceTypes.Add(ENLVoice::PASSIVE, TEXT("PASSIVE"));

    // label types
    labelTypes.Add(ENLLabel::UNKNOWN, TEXT("UNKNOWN"));
    labelTypes.Add(ENLLabel::ABBREV, TEXT("ABBREV"));
    labelTypes.Add(ENLLabel::ACOMP, TEXT("ACOMP"));
    labelTypes.Add(ENLLabel::ADVCL, TEXT("ADVCL"));
    labelTypes.Add(ENLLabel::ADVMOD, TEXT("ADVMOD"));
    labelTypes.Add(ENLLabel::AMOD, TEXT("AMOD"));
    labelTypes.Add(ENLLabel::APPOS, TEXT("APPOS"));
    labelTypes.Add(ENLLabel::ATTR, TEXT("ATTR"));
    labelTypes.Add(ENLLabel::AUX, TEXT("AUX"));
    labelTypes.Add(ENLLabel::AUXPASS, TEXT("AUXPASS"));
    labelTypes.Add(ENLLabel::CC, TEXT("CC"));
    labelTypes.Add(ENLLabel::CCOMP, TEXT("CCOMP"));
    labelTypes.Add(ENLLabel::CONJ, TEXT("CONJ"));
    labelTypes.Add(ENLLabel::CSUBJ, TEXT("CSUBJ"));
    labelTypes.Add(ENLLabel::CSUBJPASS, TEXT("CSUBJPASS"));
    labelTypes.Add(ENLLabel::DEP, TEXT("DEP"));
    labelTypes.Add(ENLLabel::DET, TEXT("DET"));
    labelTypes.Add(ENLLabel::DISCOURSE, TEXT("DISCOURSE"));
    labelTypes.Add(ENLLabel::DOBJ, TEXT("DOBJ"));
    labelTypes.Add(ENLLabel::EXPL, TEXT("EXPL"));
    labelTypes.Add(ENLLabel::GOESWITH, TEXT("GOESWITH"));
    labelTypes.Add(ENLLabel::IOBJ, TEXT("IOBJ"));
    labelTypes.Add(ENLLabel::MARK, TEXT("MARK"));
    labelTypes.Add(ENLLabel::MWE, TEXT("MWE"));
    labelTypes.Add(ENLLabel::MWV, TEXT("MWV"));
    labelTypes.Add(ENLLabel::NEG, TEXT("NEG"));
    labelTypes.Add(ENLLabel::NN, TEXT("NN"));
    labelTypes.Add(ENLLabel::NPADVMOD, TEXT("NPADVMOD"));
    labelTypes.Add(ENLLabel::NSUBJ, TEXT("NSUBJ"));
    labelTypes.Add(ENLLabel::NSUBJPASS, TEXT("NSUBJPASS"));
    labelTypes.Add(ENLLabel::NUM, TEXT("NUM"));
    labelTypes.Add(ENLLabel::NUMBER, TEXT("NUMBER"));
    labelTypes.Add(ENLLabel::P, TEXT("P"));
    labelTypes.Add(ENLLabel::PARATAXIS, TEXT("PARATAXIS"));
    labelTypes.Add(ENLLabel::PARTMOD, TEXT("PARTMOD"));
    labelTypes.Add(ENLLabel::PCOMP, TEXT("PCOMP"));
    labelTypes.Add(ENLLabel::POBJ, TEXT("POBJ"));
    labelTypes.Add(ENLLabel::POSS, TEXT("POSS"));
    labelTypes.Add(ENLLabel::POSTNEG, TEXT("POSTNEG"));
    labelTypes.Add(ENLLabel::PRECOMP, TEXT("PRECOMP"));
    labelTypes.Add(ENLLabel::PRECONJ, TEXT("PRECONJ"));
    labelTypes.Add(ENLLabel::PREDET, TEXT("PREDET"));
    labelTypes.Add(ENLLabel::PREF, TEXT("PREF"));
    labelTypes.Add(ENLLabel::PREP, TEXT("PREP"));
    labelTypes.Add(ENLLabel::PRONL, TEXT("PRONL"));
    labelTypes.Add(ENLLabel::PRT, TEXT("PRT"));
    labelTypes.Add(ENLLabel::PS, TEXT("PS"));
    labelTypes.Add(ENLLabel::QUANTMOD, TEXT("QUANTMOD"));
    labelTypes.Add(ENLLabel::RCMOD, TEXT("RCMOD"));
    labelTypes.Add(ENLLabel::RCMODREL, TEXT("RCMODREL"));
    labelTypes.Add(ENLLabel::RDROP, TEXT("RDROP"));
    labelTypes.Add(ENLLabel::REF, TEXT("REF"));
    labelTypes.Add(ENLLabel::REMNANT, TEXT("REMNANT"));
    labelTypes.Add(ENLLabel::REPARANDUM, TEXT("REPARANDUM"));
    labelTypes.Add(ENLLabel::ROOT, TEXT("ROOT"));
    labelTypes.Add(ENLLabel::SNUM, TEXT("SNUM"));
    labelTypes.Add(ENLLabel::SUFF, TEXT("SUFF"));
    labelTypes.Add(ENLLabel::TMOD, TEXT("TMOD"));
    labelTypes.Add(ENLLabel::TOPIC, TEXT("TOPIC"));
    labelTypes.Add(ENLLabel::VMOD, TEXT("VMOD"));
    labelTypes.Add(ENLLabel::VOCATIVE, TEXT("VOCATIVE"));
    labelTypes.Add(ENLLabel::XCOMP, TEXT("XCOMP"));
    labelTypes.Add(ENLLabel::SUFFIX, TEXT("SUFFIX"));
    labelTypes.Add(ENLLabel::TITLE, TEXT("TITLE"));
    labelTypes.Add(ENLLabel::ADVPHMOD, TEXT("ADVPHMOD"));
    labelTypes.Add(ENLLabel::AUXCAUS, TEXT("AUXCAUS"));
    labelTypes.Add(ENLLabel::AUXVV, TEXT("AUXVV"));
    labelTypes.Add(ENLLabel::DTMOD, TEXT("DTMOD"));
    labelTypes.Add(ENLLabel::FOREIGN, TEXT("FOREIGN"));
    labelTypes.Add(ENLLabel::KW, TEXT("KW"));
    labelTypes.Add(ENLLabel::LIST, TEXT("LIST"));
    labelTypes.Add(ENLLabel::NOMC, TEXT("NOMC"));
    labelTypes.Add(ENLLabel::NOMCSUBJ, TEXT("NOMCSUBJ"));
    labelTypes.Add(ENLLabel::NOMCSUBJPASS, TEXT("NOMCSUBJPASS"));
    labelTypes.Add(ENLLabel::NUMC, TEXT("NUMC"));
    labelTypes.Add(ENLLabel::COP, TEXT("COP"));
    labelTypes.Add(ENLLabel::DISLOCATED, TEXT("DISLOCATED"));
    labelTypes.Add(ENLLabel::ASP, TEXT("ASP"));
    labelTypes.Add(ENLLabel::GMOD, TEXT("GMOD"));
    labelTypes.Add(ENLLabel::GOBJ, TEXT("GOBJ"));
    labelTypes.Add(ENLLabel::INFMOD, TEXT("INFMOD"));
    labelTypes.Add(ENLLabel::MES, TEXT("MES"));
    labelTypes.Add(ENLLabel::NCOMP, TEXT("NCOMP"));

}
