/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef JUCE_VSTCOMMON_H_INCLUDED
#define JUCE_VSTCOMMON_H_INCLUDED

//==============================================================================
struct SpeakerMappings  : private AudioChannelSet // (inheritance only to give easier access to items in the namespace)
{
    struct Mapping
    {
        int32 vst2;
        ChannelType channels[13];

        bool matches (const Array<ChannelType>& chans) const noexcept
        {
            const int n = sizeof (channels) / sizeof (ChannelType);

            for (int i = 0; i < n; ++i)
            {
                if (channels[i] == unknown)  return (i == chans.size());
                if (i == chans.size())       return (channels[i] == unknown);

                if (channels[i] != chans.getUnchecked(i))
                    return false;
            }

            return true;
        }
    };

    static AudioChannelSet vstArrangementTypeToChannelSet (int32 arr, int fallbackNumChannels)
    {
        for (const Mapping* m = getMappings(); m->vst2 != vstSpeakerConfigTypeEmpty; ++m)
        {
            if (m->vst2 == arr)
            {
                AudioChannelSet s;

                for (int i = 0; m->channels[i] != 0; ++i)
                    s.addChannel (m->channels[i]);

                return s;
            }
        }

        return AudioChannelSet::discreteChannels (fallbackNumChannels);
    }

    static AudioChannelSet vstArrangementTypeToChannelSet (const VstSpeakerConfiguration& arr)
    {
        return vstArrangementTypeToChannelSet (arr.type, arr.numberOfChannels);
    }

    static int32 channelSetToVstArrangementType (AudioChannelSet channels)
    {
        Array<AudioChannelSet::ChannelType> chans (channels.getChannelTypes());

        if (channels == AudioChannelSet::disabled())
            return vstSpeakerConfigTypeEmpty;

        for (const Mapping* m = getMappings(); m->vst2 != vstSpeakerConfigTypeEmpty; ++m)
            if (m->matches (chans))
                return m->vst2;

        return vstSpeakerConfigTypeUser;
    }

    static void channelSetToVstArrangement (const AudioChannelSet& channels, VstSpeakerConfiguration& result)
    {
        result.type = channelSetToVstArrangementType (channels);
        result.numberOfChannels = channels.size();

        for (int i = 0; i < result.numberOfChannels; ++i)
        {
            VstIndividualSpeakerInfo& speaker = result.speakers[i];

            zeromem (&speaker, sizeof (VstIndividualSpeakerInfo));
            speaker.type = getSpeakerType (channels.getTypeOfChannel (i));
        }
    }

    static const Mapping* getMappings() noexcept
    {
        static const Mapping mappings[] =
        {
            { vstSpeakerConfigTypeMono,           { centre, unknown } },
            { vstSpeakerConfigTypeLR,         { left, right, unknown } },
            { vstSpeakerConfigTypeLsRs, { leftSurround, rightSurround, unknown } },
            { vstSpeakerConfigTypeLcRc,   { leftCentre, rightCentre, unknown } },
            { vstSpeakerConfigTypeSlSr,     { leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeCLfe,     { centre, subbass, unknown } },
            { vstSpeakerConfigTypeLRC,         { left, right, centre, unknown } },
            { vstSpeakerConfigTypeLRS,        { left, right, surround, unknown } },
            { vstSpeakerConfigTypeLRCLfe,         { left, right, centre, subbass, unknown } },
            { vstSpeakerConfigTypeLRLfeS,        { left, right, subbass, surround, unknown } },
            { vstSpeakerConfigTypeLRCS,         { left, right, centre, surround, unknown } },
            { vstSpeakerConfigTypeLRLsRs,        { left, right, leftSurround, rightSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeS,         { left, right, centre, subbass, surround, unknown } },
            { vstSpeakerConfigTypeLRLfeLsRs,        { left, right, subbass, leftSurround, rightSurround, unknown } },
            { vstSpeakerConfigTypeLRCLsRs,             { left, right, centre, leftSurround, rightSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRs,             { left, right, centre, subbass, leftSurround, rightSurround, unknown } },
            { vstSpeakerConfigTypeLRCLsRsCs,         { left, right, centre, leftSurround, rightSurround, surround, unknown } },
            { vstSpeakerConfigTypeLRLsRsSlSr,        { left, right, leftSurround, rightSurround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsCs,         { left, right, centre, subbass, leftSurround, rightSurround, surround, unknown } },
            { vstSpeakerConfigTypeLRLfeLsRsSlSr,        { left, right, subbass, leftSurround, rightSurround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLsRsLcRc,         { left, right, centre, leftSurround, rightSurround, topFrontLeft, topFrontRight, unknown } },
            { vstSpeakerConfigTypeLRCLsRsSlSr,        { left, right, centre, leftSurround, rightSurround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsLcRc,         { left, right, centre, subbass, leftSurround, rightSurround, topFrontLeft, topFrontRight, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsSlSr,        { left, right, centre, subbass, leftSurround, rightSurround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLsRsLcRcCs,         { left, right, centre, leftSurround, rightSurround, topFrontLeft, topFrontRight, surround, unknown } },
            { vstSpeakerConfigTypeLRCLsRsCsSlSr,        { left, right, centre, leftSurround, rightSurround, surround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsLcRcCs,         { left, right, centre, subbass, leftSurround, rightSurround, topFrontLeft, topFrontRight, surround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsCsSlSr,        { left, right, centre, subbass, leftSurround, rightSurround, surround, leftRearSurround, rightRearSurround, unknown } },
            { vstSpeakerConfigTypeLRCLfeLsRsTflTfcTfrTrlTrrLfe2,            { left, right, centre, subbass, leftSurround, rightSurround, topFrontLeft, topFrontCentre, topFrontRight, topRearLeft, topRearRight, subbass2, unknown } },
            { vstSpeakerConfigTypeEmpty,          { unknown } }
        };

        return mappings;
    }

    static inline int32 getSpeakerType (AudioChannelSet::ChannelType type) noexcept
    {
        switch (type)
        {
            case AudioChannelSet::left:              return vstIndividualSpeakerTypeLeft;
            case AudioChannelSet::right:             return vstIndividualSpeakerTypeRight;
            case AudioChannelSet::centre:            return vstIndividualSpeakerTypeCentre;
            case AudioChannelSet::subbass:           return vstIndividualSpeakerTypeSubbass;
            case AudioChannelSet::leftSurround:      return vstIndividualSpeakerTypeLeftSurround;
            case AudioChannelSet::rightSurround:     return vstIndividualSpeakerTypeRightSurround;
            case AudioChannelSet::leftCentre:        return vstIndividualSpeakerTypeLeftCentre;
            case AudioChannelSet::rightCentre:       return vstIndividualSpeakerTypeRightCentre;
            case AudioChannelSet::surround:          return vstIndividualSpeakerTypeSurround;
            case AudioChannelSet::leftRearSurround:  return vstIndividualSpeakerTypeLeftRearSurround;
            case AudioChannelSet::rightRearSurround: return vstIndividualSpeakerTypeRightRearSurround;
            case AudioChannelSet::topMiddle:         return vstIndividualSpeakerTypeTopMiddle;
            case AudioChannelSet::topFrontLeft:      return vstIndividualSpeakerTypeTopFrontLeft;
            case AudioChannelSet::topFrontCentre:    return vstIndividualSpeakerTypeTopFrontCentre;
            case AudioChannelSet::topFrontRight:     return vstIndividualSpeakerTypeTopFrontRight;
            case AudioChannelSet::topRearLeft:       return vstIndividualSpeakerTypeTopRearLeft;
            case AudioChannelSet::topRearCentre:     return vstIndividualSpeakerTypeTopRearCentre;
            case AudioChannelSet::topRearRight:      return vstIndividualSpeakerTypeTopRearRight;
            case AudioChannelSet::subbass2:          return vstIndividualSpeakerTypeSubbass2;
            default: break;
        }

        return 0;
    }

    static inline AudioChannelSet::ChannelType getChannelType (int32 type) noexcept
    {
        switch (type)
        {
            case vstIndividualSpeakerTypeLeft:                 return AudioChannelSet::left;
            case vstIndividualSpeakerTypeRight:                return AudioChannelSet::right;
            case vstIndividualSpeakerTypeCentre:               return AudioChannelSet::centre;
            case vstIndividualSpeakerTypeSubbass:              return AudioChannelSet::subbass;
            case vstIndividualSpeakerTypeLeftSurround:         return AudioChannelSet::leftSurround;
            case vstIndividualSpeakerTypeRightSurround:        return AudioChannelSet::rightSurround;
            case vstIndividualSpeakerTypeLeftCentre:           return AudioChannelSet::leftCentre;
            case vstIndividualSpeakerTypeRightCentre:          return AudioChannelSet::rightCentre;
            case vstIndividualSpeakerTypeSurround:             return AudioChannelSet::surround;
            case vstIndividualSpeakerTypeLeftRearSurround:     return AudioChannelSet::leftRearSurround;
            case vstIndividualSpeakerTypeRightRearSurround:    return AudioChannelSet::rightRearSurround;
            case vstIndividualSpeakerTypeTopMiddle:            return AudioChannelSet::topMiddle;
            case vstIndividualSpeakerTypeTopFrontLeft:         return AudioChannelSet::topFrontLeft;
            case vstIndividualSpeakerTypeTopFrontCentre:       return AudioChannelSet::topFrontCentre;
            case vstIndividualSpeakerTypeTopFrontRight:        return AudioChannelSet::topFrontRight;
            case vstIndividualSpeakerTypeTopRearLeft:          return AudioChannelSet::topRearLeft;
            case vstIndividualSpeakerTypeTopRearCentre:        return AudioChannelSet::topRearCentre;
            case vstIndividualSpeakerTypeTopRearRight:         return AudioChannelSet::topRearRight;
            case vstIndividualSpeakerTypeSubbass2:             return AudioChannelSet::subbass2;
            default: break;
        }

        return AudioChannelSet::unknown;
    }
};

#endif   // JUCE_VSTCOMMON_H_INCLUDED
