#ifndef reactive_component_h
#define reactive_component_h

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "./json.hpp"

namespace jgod { namespace reactive {
#pragma mark - Types
  class Component;
  typedef std::shared_ptr<Component> SharedComponent;
  // type ReactNodeList = ReactNode | ReactEmpty
  typedef std::vector<SharedComponent> NodeList;

  typedef jgod_nlohmann::json JSON;
  typedef JSON State;
  typedef JSON Props;
  typedef std::function<void(const State &prevState,
                             const Props &currentProps)> UpdateCb;
  typedef std::function<const State(const State &prevState,
                                    const Props &currentProps)> ReturnedUpdateCb;

#pragma mark - Component
  class Component {
  public:
    Component() : Component("", {}, {}){}
    Component(const std::string key,
              const Props props,
              const NodeList children) :
    _key(key), _props(props) {addChildren(children);} // componentDidMount()
    virtual ~Component() {} // componentWillUnmount()

#pragma mark - Updating
    ////////////////////////////////////////////////////////////////////////////////////
    /**
     * Invoked before rendering when new props or state are being received.
     * This method is not called for the initial render or when forceUpdate is used.
     * Use this as an opportunity to return false when you're certain that the
     * transition to the new props and state will not require a component update.
     *
     * If shouldComponentUpdate returns false, then render() will be completely
     * skipped until the next state change.
     * In addition, componentWillUpdate and componentDidUpdate will not be called.
     *
     * By default, shouldComponentUpdate always returns true to prevent subtle bugs
     * when state is mutated in place, but if you are careful to always treat state
     * as immutable and to read only from props and state in render() then you can
     * override shouldComponentUpdate with an implementation that compares the old
     * props and state to their replacements.
     *
     * If performance is a bottleneck, especially with dozens or hundreds of components,
     * use shouldComponentUpdate to speed up your app.
     *
     * @param[in] nextProps
     * @param[in] nextState
     * @returns bool
     * @see https://facebook.github.io/react/docs/component-specs.html#updating-shouldcomponentupdate
     */
    virtual bool shouldComponentUpdate(const Props&, const State&) {return true;}

    /**
     * Invoked immediately before rendering when new props or state are being received.
     * This method is not called for the initial render.
     * Use this as an opportunity to perform preparation before an update occurs.
     *
     * @param[in] nextProps
     * @param[in] nextState
     * @see https://facebook.github.io/react/docs/component-specs.html#updating-componentwillupdate
     */
    virtual void componentWillUpdate(const Props&, const State&){}

    /**
     * Invoked immediately after the component's updates are flushed to the DOM.
     * This method is not called for the initial render.
     * Use this as an opportunity to operate on the DOM when the component has been updated.
     *
     * @param[in] prevProps
     * @param[in] prevState
     * @see https://facebook.github.io/react/docs/component-specs.html#updating-componentdidupdate
     */
    virtual void componentDidUpdate(const Props&, const State&){}

    inline void forceUpdate() {render(true);}
    ////////////////////////////////////////////////////////////////////////////

#pragma mark - State
    ////////////////////////////////////////////////////////////////////////////
    /**
     * Performs a shallow merge of nextState into current state.
     * This is the primary method you use to trigger UI updates from event handlers
     * and server request callbacks.
     *
     * @param[in] nextState
     * @param[in] cb(prevState, currentProps)
     * @see https://facebook.github.io/react/docs/component-api.html#setstate
     */
    inline void setState(const State &nextState,
                         const UpdateCb &cb = [](const State&,
                                                 const Props&){}) {
      auto prevState = _state;

      // Shallow merge
      auto newState = _state;
      for (auto it = std::begin(nextState); it != std::end(nextState); ++it) {
        newState[it.key()] = it.value();
      }

      if (shouldComponentUpdate(_props, newState)) {
        componentWillUpdate(_props, newState);
        render(true);
        componentDidUpdate(_props, prevState);
      }
      _state = newState;
      cb(prevState, _props);
    }
    /**
     * Performs a shallow merge of nextState into current state.
     * This is the primary method you use to trigger UI updates from event handlers
     * and server request callbacks.
     *
     * @see https://facebook.github.io/react/docs/component-api.html#setstate
     */
    inline void setState(const ReturnedUpdateCb &updateCb,
                         const UpdateCb& = [](const State&, const Props&){}) {
      setState(updateCb(_state, _props));
    }
    ////////////////////////////////////////////////////////////////////////////

#pragma mark - Rendering
    // Render functions need to be pure!
    virtual void render(bool force = false) const = 0;

#pragma mark - Children
    inline void addChild(SharedComponent const component) {
      if (!component) return;
      // Don't allow duplicates.
      auto it = std::find_if(std::begin(_children),
                             std::end(_children),
                             [&](const SharedComponent &c) {
        return (c && c->getKey() == component->getKey());
      });
      if (it == _children.end()) {
        _children.push_back(component);
        component->setParent(this);
      }
    }
    inline void addChildren(NodeList components) {
      for (auto &child : components) {
        if (!child) continue;
        addChild(child);
      }
    }
    inline void removeChild(SharedComponent const component) {
      if (!component) return;
      removeChild(component->getKey());
    }
    inline void removeChild(const std::string &key) {
      if (_children.empty()) return;
      _children.erase(std::remove_if(std::begin(_children),
                                     std::end(_children),
                                     [=](const SharedComponent &c) {
        return (c && c->getKey() == key);
      }));
    }
    inline void removeChildren() {_children.clear();}

#pragma mark - Getters and Setters
    // Props shouldn't be modified directly!
    inline const Props &getProps() const {return _props;}
    inline std::string getKey() const {return _key;}
    // State shouldn't be modified directly!
    inline const State &getState() const {return _state;}
    inline const NodeList &getChildren() const {return _children;}
    inline Component* const getParent() const {return _parent;}
    inline void setParent(Component* const parent) {_parent = parent;}

  protected:
    Props _props = JSON(); // {children: NodeList, className: string, etc.}
    std::string _key = ""; // string | boolean | number | null
    /** @see: https://facebook.github.io/react/docs/more-about-refs.html */
    State _state = JSON();
    NodeList _children;
    Component *_parent = nullptr;
  };

  typedef std::shared_ptr<Component> SharedComponent;
}}
#endif /* reactive_component_h */
