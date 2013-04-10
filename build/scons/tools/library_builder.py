
import os

class SelectableProxy:
    """ Adapted from SCons.Util.Proxy """
    def __init__(self, subject_list,selected_subject):
        """Wrap an object as a Proxy object"""
        self.__subject = subject_list
        self.__selected_subject = selected_subject

    def __getattr__(self, name):
        """Retrieve an attribute from the wrapped object.  If the named
           attribute doesn't exist, AttributeError is raised"""
        return getattr(self.proxy_get_selected_subject(), name)

    def proxy_set_selected_subject(self,index):
        self.__selected_subject=index

    def proxy_get_selected_subject(self):
        """Retrieve the entire wrapped object"""
        return self.__subject[self.__selected_subject]

    def __cmp__(self, other):
        if issubclass(other.__class__, self.proxy_get_selected_subject().__class__):
            return cmp(self.proxy_get_selected_subject(), other)
        return cmp(self.__dict__, other.__dict__)

class LibraryBuilderProxy(SelectableProxy):
    def __init__(self,static_builder,shared_builder,shared=None):
        SelectableProxy.__init__(self,[static_builder,shared_builder],0 if shared is None else shared)
        self.make_shared=shared

    def __call__(self, env, target, source,*args, **kwargs):
        # Use the correct proxy builder
        
        shared=self.make_shared
        if shared is not None:
            if 'shared' in kwargs:
                raise TypeError('You cannot override the "shared" keyword argument, as it was already set during construction.')
        else:
            if 'shared' not in kwargs:
                # assume it from the environment
                shared=env.IsShared()
            else:
                shared=kwargs['shared']
                del kwargs['shared']

        self.proxy_set_selected_subject(1 if shared else 0)
        builder=self.proxy_get_selected_subject()

        if env.IsUsing():
            # If using, we just output the symbols, but don't build anything
            env=env.Override(kwargs)
            target_list, source_list=builder._create_nodes(env,target,source)

            lib_list=[]
            if shared:
                env=env.Override({'CCFLAGS':'-fPIC'}) # for shared libs, we want position independent code
                path_list=[]
                for obj in target_list:
                    split_path = os.path.split(str(obj))
                    path_name=split_path[0]
                    if not path_name:
                        path_name='.'
                    if path_name not in path_list:
                        path_list.append(path_name)

                    lib_name=split_path[1]
                    orig_prefix=env.RemoveOutputPrefix(env['SHLIBPREFIX'])
                    suffix=env['SHLIBSUFFIX']
                    if orig_prefix and lib_name.startswith(orig_prefix):
                        lib_name=lib_name[len(orig_prefix):]
                    if suffix and lib_name.endswith(suffix):
                        lib_name=lib_name[:-len(suffix)]

                    if lib_name not in lib_list:
                        lib_list.append(lib_name)
                for i in range(len(path_list)):
                    path_list[i] = env.Dir(path_list[i])
                env.AppendUnique(LIBPATH=path_list)
                env.AppendUnique(RPATH=path_list)
            else:
                lib_list = target_list

            env.Append(LIBS=lib_list)
        else:
			# We actually need to build it
			target_list=builder(env,target,source,*args,**kwargs)
			if 'LIB_DIR' in env.Dictionary():
				target_list += env.Install('$LIB_DIR', sources=[target_list])
        return target_list

def generate(env):
    """SCons entry point for this tool."""

    # Determine if this environment is using the new VariantDir approach
    # to building files.
    if 'TARGET_ROOT' in env.Dictionary():
        env.Replace(
                EXE_DIR='$TARGET_ROOT/exe',
                LIB_DIR='$TARGET_ROOT/lib',
                OBJ_DIR='$TARGET_ROOT/obj',
                TEST_DIR='$TARGET_ROOT/tests',
                )
        env.Append(
                LIBPATH=['$LIB_DIR'],
                LIBS=[],
                )

    # Add the pseudo-builders.
    env['BUILDERS']['NXLibrary'] = LibraryBuilderProxy(env.StaticLibrary.builder,env.SharedLibrary.builder) 
    env['BUILDERS']['NXStaticLibrary'] = LibraryBuilderProxy(env.StaticLibrary.builder,None,shared=False) 
    env['BUILDERS']['NXSharedLibrary'] = LibraryBuilderProxy(None,env.SharedLibrary.builder,shared=True) 

def exists():
	return 1
