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

static StringArray readDeadMansPedalFile (const File& file)
{
    StringArray lines;
    file.readLines (lines);
    lines.removeEmptyStrings();
    return lines;
}

PluginDirectoryScanner::PluginDirectoryScanner (KnownPluginList& listToAddTo,
                                                AudioPluginFormat& formatToLookFor,
                                                FileSearchPath directoriesToSearch,
                                                const bool recursive,
                                                const File& deadMansPedal,
                                                bool allowPluginsWhichRequireAsynchronousInstantiation)
    : list (listToAddTo),
      format (formatToLookFor),
      deadMansPedalFile (deadMansPedal),
      allowAsync (allowPluginsWhichRequireAsynchronousInstantiation)
{
    directoriesToSearch.removeRedundantPaths();

    filesOrIdentifiersToScan = format.searchPathsForPlugins (directoriesToSearch, recursive, allowAsync);

    // If any plugins have crashed recently when being loaded, move them to the
    // end of the list to give the others a chance to load correctly..
    for (auto& crashed : readDeadMansPedalFile (deadMansPedalFile))
        for (int j = filesOrIdentifiersToScan.size(); --j >= 0;)
            if (crashed == filesOrIdentifiersToScan[j])
                filesOrIdentifiersToScan.move (j, -1);

    applyBlacklistingsFromDeadMansPedal (listToAddTo, deadMansPedalFile);
    nextIndex.set (filesOrIdentifiersToScan.size());
}

PluginDirectoryScanner::~PluginDirectoryScanner()
{
    list.scanFinished();
}

//==============================================================================
String PluginDirectoryScanner::getNextPluginFileThatWillBeScanned() const
{
    return format.getNameOfPluginFromIdentifier (filesOrIdentifiersToScan [nextIndex.get() - 1]);
}

void PluginDirectoryScanner::updateProgress()
{
    progress = (1.0f - nextIndex.get() / (float) filesOrIdentifiersToScan.size());
}

bool PluginDirectoryScanner::scanNextFile (bool dontRescanIfAlreadyInList,
                                           String& nameOfPluginBeingScanned)
{
    const int index = --nextIndex;

    if (index >= 0)
    {
        auto file = filesOrIdentifiersToScan [index];

        if (file.isNotEmpty() && ! (dontRescanIfAlreadyInList && list.isListingUpToDate (file, format)))
        {
            nameOfPluginBeingScanned = format.getNameOfPluginFromIdentifier (file);

            OwnedArray<PluginDescription> typesFound;

            // Add this plugin to the end of the dead-man's pedal list in case it crashes...
            auto crashedPlugins = readDeadMansPedalFile (deadMansPedalFile);
            crashedPlugins.removeString (file);
            crashedPlugins.add (file);
            setDeadMansPedalFile (crashedPlugins);

            list.scanAndAddFile (file, dontRescanIfAlreadyInList, typesFound, format);

            // Managed to load without crashing, so remove it from the dead-man's-pedal..
            crashedPlugins.removeString (file);
            setDeadMansPedalFile (crashedPlugins);

            if (typesFound.size() == 0 && ! list.getBlacklistedFiles().contains (file))
                failedFiles.add (file);
        }
    }

    updateProgress();
    return index > 0;
}

bool PluginDirectoryScanner::skipNextFile()
{
    updateProgress();
    return --nextIndex > 0;
}

void PluginDirectoryScanner::setDeadMansPedalFile (const StringArray& newContents)
{
    if (deadMansPedalFile.getFullPathName().isNotEmpty())
        deadMansPedalFile.replaceWithText (newContents.joinIntoString ("\n"), true, true);
}

void PluginDirectoryScanner::applyBlacklistingsFromDeadMansPedal (KnownPluginList& list, const File& file)
{
    // If any plugins have crashed recently when being loaded, move them to the
    // end of the list to give the others a chance to load correctly..
    for (auto& crashedPlugin : readDeadMansPedalFile (file))
        list.addToBlacklist (crashedPlugin);
}
