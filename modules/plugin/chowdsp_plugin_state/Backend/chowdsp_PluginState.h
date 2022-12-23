#pragma once

namespace chowdsp
{
/** Base class for managing a plugin's state. */
class PluginState
{
public:
    PluginState() = default;
    virtual ~PluginState() = default;

    /** Initialises the plugin state with a given set of parameters. */
    void initialise (ParamHolder& parameters,
                     juce::AudioProcessor* processor = nullptr,
                     juce::UndoManager* um = nullptr)
    {
        undoManager = um;
        listeners.emplace (parameters);
        if (processor != nullptr)
            parameters.connectParametersToProcessor (*processor);
    }

    /** Serializes the plugin state to the given MemoryBlock */
    virtual void serialize (juce::MemoryBlock& data) const = 0;

    /** Deserializes the plugin state from the given MemoryBlock */
    virtual void deserialize (const juce::MemoryBlock& data) = 0;

    /**
     * Adds a parameter listener which will be called on either the message
     * thread or the audio thread (you choose!). Listeners should have the
     * signature void().
     */
    template <typename... ListenerArgs>
    [[nodiscard]] ScopedCallback addParameterListener (const juce::RangedAudioParameter& param,
                                                       ParameterListenerThread listenerThread,
                                                       ListenerArgs&&... args)
    {
        return listeners->addParameterListener (param, listenerThread, std::forward<ListenerArgs...> (args...));
    }

    /** Returns the plugin parameter listeners. */
    auto& getParameterListeners() { return *listeners; }

    /**
     * Adds a listener to some field of the plugin's non-parameter state.
     * The listener will be called on whichever thread the state value is
     * mutated on. Listeners should have the signature void().
     */
    template <typename... ListenerArgs>
    [[nodiscard]] ScopedCallback addNonParameterListener (StateValueBase& stateValue,
                                                          ListenerArgs&&... args) const
    {
        return stateValue.changeBroadcaster.connect (std::forward<ListenerArgs...> (args...));
    }

    juce::UndoManager* undoManager = nullptr;

private:
    std::optional<ParameterListeners> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginState)
};
} // namespace chowdsp
